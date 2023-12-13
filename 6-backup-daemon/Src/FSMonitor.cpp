#include "FSMonitor.hpp"

using namespace HwBackup;
using namespace TnHelpers;

FSMonitor::FSMonitor(size_t eventCapacity, Logger* loggerPtr)
    : Fd{FdInit()}, Buffer{BufferInit(eventCapacity)}, LoggerPtr{loggerPtr} {
  if (!loggerPtr) THROW("Logger pointer is NULL");
}

FSMonitor::~FSMonitor() { UnregisterAll(); }

void FSMonitor::Register(const std::string& eventPath, uint32_t mask) {
  std::string absPath = RootPath + "/" + eventPath;
  int wd = inotify_add_watch(Fd.Get(), absPath.c_str(), mask);
  if (wd < 0) THROW_ERRNO("inotify_add_watch() with path=\"" + absPath + "\"");

  Cache.Add({wd, eventPath});
}

void FSMonitor::Unregister(WDCache::ListIt iter) {
  if (iter == Cache.End()) THROW("Entry not found");

  int ret = RmWatch(iter->Wd);
  if (ret < 0) THROW_ERRNO("inotify_rm_watch()");

  Cache.Remove(iter);
}

void FSMonitor::Unregister(WDCache::WDescriptorT wd) {
  auto iter = Cache.Find(wd);
  Unregister(iter);
}

void FSMonitor::Unregister(const std::string& path) {
  auto iter = Cache.Find(path);
  Unregister(iter);
}

void FSMonitor::GetStages(PathTree& tree) {
  int len = read(Fd.Get(), Buffer.data(), Buffer.size());
  if (len < 0) THROW_ERRNO("read()");

  const inotify_event* event;
  const uint8_t* ptr = Buffer.data();

  for (; ptr < Buffer.data() + len; ptr += sizeof(inotify_event) + event->len) {
    event = reinterpret_cast<const inotify_event*>(ptr);

    if (event->mask & IN_IGNORED) continue;  // Ignore the IN_INGNORED event

    auto cached = Cache.Find(event->wd);
    if (cached == Cache.End()) THROW("Watch desciptor is not cached");

    HandleEvent(*event, cached->Path);
    tree.AddPath(cached->Path, event->name);
  }
}

FileDescriptor FSMonitor::FdInit() {
  int ret = inotify_init();
  if (ret < 0) THROW_ERRNO("inotify_init()");

  return ret;
}

auto FSMonitor::BufferInit(size_t eventCapacity) -> BufferT {
  return BufferT(eventCapacity * InotifyEventMaxSize, 0);
}

int FSMonitor::RmWatch(int wd) const { return inotify_rm_watch(Fd.Get(), wd); }

void FSMonitor::RegisterAt(Selector& selector) {
  if (IsSelected) THROW("Already selected");

  SelectorId = selector.Register(Fd.Get(), POLLIN);
  IsSelected = true;
}

bool FSMonitor::DataReady(const Selector& selector) const {
  if (!IsSelected) THROW("Not selected");
  return selector.GetEvents(SelectorId) & POLLIN;
}

void FSMonitor::UnregisterAll() {
  for (auto it = Cache.Begin(); it != Cache.End();) {
    int ret = RmWatch(it->Wd);
    if (ret < 0)
      LOG_WARN(GetLogger(), "inotify_rm_watch(" << it->Wd << "[" << it->Path
                                                << "]): " << strerror(errno));

    auto oldIt = it;
    it++;

    Cache.Remove(oldIt);
  }
}

void FSMonitor::HandleEvent(const inotify_event& event,
                            const std::string& path) {
  if (!(event.mask & IN_ISDIR)) return;

  std::string eventPath = path + "/" + event.name;

  if (event.mask & IN_CREATE) {  // New directory created
    LOG_INFO(GetLogger(), "New directory created on \"" << eventPath << "\"");
    RegisterTree(eventPath);
  }

  if (event.mask & IN_DELETE) {  // Directory deleted
    LOG_INFO(GetLogger(), "Directory deleted on \"" << eventPath << "\"");

    Cache.Remove(eventPath);  // No need to unregister, watch will be removed
                              // explicitly via inotify (see inotify(7))
  }
}

std::vector<std::string> FSMonitor::ScanTree(const std::string& relPathRoot) {
  std::vector<std::string> BFSCache;
  BFSCache.push_back(relPathRoot);
  size_t currentInd = 0;

  auto deleter = [](DIR* dir) {
    int ret = closedir(dir);
    if (ret < 0) THROW_ERRNO("closedir()");
  };

  do {
    std::string relPath = BFSCache[currentInd];
    std::string absPath = RootPath + "/" + relPath;
    std::unique_ptr<DIR, decltype(deleter)> dir{opendir(absPath.c_str()),
                                                deleter};

    if (!dir.get())
      THROW_ERRNO("Failed to open directory \""s + absPath + "\"");

    dirent* curDir = NULL;
    while ((curDir = readdir(dir.get())) != NULL) {
      if (strcmp(curDir->d_name, ".") == 0 || strcmp(curDir->d_name, "..") == 0)
        continue;

      if (curDir->d_type == DT_REG) continue;

      std::string entryPath = relPath + "/" + curDir->d_name;

      if (curDir->d_type != DT_DIR) {
        LOG_WARN(GetLogger(),
                 "File type of \"" << entryPath << "\" is not supported");
        continue;
      }

      BFSCache.push_back(entryPath);
    }
    ++currentInd;
  } while (currentInd != BFSCache.size());

  return BFSCache;
}

void FSMonitor::RegisterTree(const std::string& relPathRoot) {
  auto dirs = ScanTree(relPathRoot);
  RegisterDirectories(dirs);
}

void FSMonitor::RegisterDirectories(const std::vector<std::string>& dirs) {
  auto iter = dirs.cbegin();

  uint32_t mask = IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVE | IN_EXCL_UNLINK;

  for (; iter != dirs.cend(); ++iter) {
    LOG_INFO(GetLogger(), "Registering directory \"" << *iter << "\"");

    try {
      Register(*iter, mask);
    } catch (std::exception& e) {
      LOG_ERROR(GetLogger(),
                "Caught exception during registration: " << e.what());
      for (; iter != dirs.cbegin(); --iter) Unregister(*iter);
      throw;
    }
  }
}

Logger& FSMonitor::GetLogger() {
  assert(LoggerPtr);
  return *LoggerPtr;
}

void FSMonitor::Start(const std::string& rootPath) {
  if (Started) THROW("Already started");

  RootPath = rootPath;
  RegisterTree(".");
  Started = true;
}

void FSMonitor::Stop() {
  if (!Started) THROW("Not started");

  Started = false;
  UnregisterAll();
  RootPath = "";
}