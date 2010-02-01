#ifndef __SHINOBI_KEY_SEQUENCE_EDIT_H__
#define __SHINOBI_KEY_SEQUENCE_EDIT_H__

#include "config.h"
#include <QLineEdit>
#include <QKeyEvent>
#include <QKeySequence>

namespace shinobi {
// -------------------------------------------------------------------------- //
// KeySequenceEdit
// -------------------------------------------------------------------------- //
  class KeySequenceEdit: public QLineEdit {
    Q_OBJECT;
  public:
    KeySequenceEdit(QKeySequence keySequence = QKeySequence(), QWidget* parent = 0);

    QKeySequence keySequence() const {
      return mKeySequence;
    }

  protected:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void focusOutEvent(QFocusEvent* event);

  private:
    enum State {
      Init,
      Constructing,
      Fixing
    };

    void updateKeySequence(QKeySequence keySequence);
    void updateKeySequence(int key, int modifiers);
    static QKeySequence keysToKeySequence(int key, int modifiers);
    static bool isValid(int key);
    static bool isModifier(int key);

    QKeySequence mKeySequence;
    State mState;
    int mCurrentKey;
    int mCurrentModifiers;
  };

} // namespace shinobi

#endif // __SHINOBI_KEY_SEQUENCE_EDIT_H__
