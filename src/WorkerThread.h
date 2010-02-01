#ifndef __SHINOBI_WORKER_THREAD_H__
#define __SHINOBI_WORKER_THREAD_H__

#include "config.h"
#include <cassert>
#include <QThread>
#include <QDir>
#include "FileFilter.h"

namespace shinobi {
// -------------------------------------------------------------------------- //
// WorkerThread
// -------------------------------------------------------------------------- //
  /** Worker thread class. 
   * Copies directory trees based on the given set of filter. */
  class WorkerThread: public QThread {
    Q_OBJECT;
  public: 
    WorkerThread(const QString& sourcePath, const QString& targetPath, const QList<FileFilter>& fileFilters): 
      mPlugged(true), mTargetDir(targetPath), mSourceDir(sourcePath), mFileFilters(fileFilters) {}

    virtual void run();

    void unplug() {
      mPlugged = false;
    }

  private:
    void copy(const QFileInfo& fileInfo);

    void deepCopy(const QFileInfo& fileInfo);

    volatile bool mPlugged;
    QDir mTargetDir;
    QDir mSourceDir;
    QList<FileFilter> mFileFilters;
  };

} // namespace shinobi

#endif // __SHINOBI_WORKER_THREAD_H__
