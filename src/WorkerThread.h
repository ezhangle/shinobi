#ifndef __SHINOBI_WORKER_THREAD_H__
#define __SHINOBI_WORKER_THREAD_H__

#include "config.h"
#include <cassert>
#include <QThread>
#include <QDir>
#include <QMutex>
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
      mPlugged(true), mTargetDir(targetPath), mSourceDir(sourcePath), mFileFilters(fileFilters), mLocked(false) {}

    virtual void run();

    void unplug() {
      mPlugged = false;
    }

    void lock() {
      assert(!mLocked);

      mMutex.lock();
      mLocked = true;
    }

    void unlock() {
      assert(mLocked);

      mLocked = false;
      mMutex.unlock();
    }

    bool locked() const {
      return mLocked;
    }

  private:
    void copy(const QFileInfo& fileInfo);

    void deepCopy(const QFileInfo& fileInfo);

    volatile bool mLocked;
    volatile bool mPlugged;
    QMutex mMutex;
    QDir mTargetDir;
    QDir mSourceDir;
    QList<FileFilter> mFileFilters;
  };

} // namespace shinobi

#endif // __SHINOBI_WORKER_THREAD_H__
