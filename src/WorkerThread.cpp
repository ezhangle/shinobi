#include "WorkerThread.h"
#include <QVector>
#include <QQueue>
#include <QFile>

namespace shinobi {
  void WorkerThread::run() {
    /* No filters => nothing to do. */
    if(mFileFilters.size() == 0)
      return;

    /* Target folder must exist. */
    if(!mTargetDir.exists())
      if(!QDir().mkpath(mTargetDir.path()))
        return;

    /* Set up queues. */
    QVector<QQueue<QFileInfo> > queues;
    queues.resize(mFileFilters.size());
    queues.front().enqueue(QFileInfo(mSourceDir.path()));

    /* Note: we'll have problems with NTFS junction points in case they are used to
     * create infinite recursion. However, we use breadth-first scanning, so it probably
     * won't affect the functionality. */

    /* Loop through filters. */
    for(int filterIndex = 0; mPlugged && filterIndex < mFileFilters.size(); filterIndex++) {
      FileFilter& filter = mFileFilters[filterIndex];
      QQueue<QFileInfo>& queue = queues[filterIndex];
      
      /* Apply current filter to each element in queue. */
      while(!queue.isEmpty()) {
        QFileInfo fileInfo = queue.dequeue();
        
        /* Lock if has been forced to. */
        if(mLocked) {
          mMutex.lock();
          mMutex.unlock();
        }

        /* Check unplugged state AFTER lock. */
        if(!mPlugged)
          break;

        /* Ignore symlinks. */
        if(fileInfo.isSymLink())
          continue;

        if(filter.matches(fileInfo)) {
          if(filter.action() == FileFilter::Scan && fileInfo.isDir()) {
            copy(fileInfo);
            foreach(const QFileInfo& childInfo, QDir(fileInfo.filePath(), "*", QDir::NoSort, QDir::Filters(QDir::AllEntries | QDir::NoDotAndDotDot)).entryInfoList())
              queues.front().enqueue(childInfo);
            
            /* If new items were added - restart from the first filter. */
            if(!queues.front().isEmpty() && filterIndex > 0) {
              filterIndex = -1;
              break;
            }
          } else if(filter.action() == FileFilter::Copy)
            deepCopy(fileInfo);
          /* Do nothing if filter.action() == FileFilter::Skip. */
        } else if(filterIndex + 1 < queues.size())
          queues[filterIndex + 1].enqueue(fileInfo);
      }
    }
  }

  void WorkerThread::copy(const QFileInfo& fileInfo) {
    QString target = mTargetDir.path() + "/" + fileInfo.filePath().right(fileInfo.filePath().size() - mSourceDir.path().size());
    if(fileInfo.isDir())
      QDir().mkpath(target);
    else
      QFile::copy(fileInfo.filePath(), target);
  }

  void WorkerThread::deepCopy(const QFileInfo& fileInfo) {
    copy(fileInfo);

    /* Copy recursively if it is a directory. */
    if(fileInfo.isDir())
      foreach(const QFileInfo& childInfo, QDir(fileInfo.filePath(), "*", QDir::NoSort, QDir::Filters(QDir::AllEntries | QDir::NoDotAndDotDot)).entryInfoList())
        deepCopy(childInfo);
  }

} // namespace shinobi
