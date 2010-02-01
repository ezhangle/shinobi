#include "KeySequenceEdit.h"
#include <QtGui>

namespace shinobi {
  KeySequenceEdit::KeySequenceEdit(QKeySequence keySequence, QWidget* parent): QLineEdit(parent), mKeySequence(keySequence), mCurrentKey(0), mCurrentModifiers(0), mState(Init) {
    updateKeySequence(keySequence);
  }

  void KeySequenceEdit::keyPressEvent(QKeyEvent* event) {
    switch(mState) {
    case Init:
      if(isValid(event->key())) {
        mState = Fixing;
        mCurrentKey = event->key();
      } else {
        mState = Constructing;
        mCurrentKey = 0;
      }
      mCurrentModifiers = event->modifiers();
      updateKeySequence(mCurrentKey, mCurrentModifiers);
      break;
    case Constructing:
      if(isValid(event->key())) {
        mState = Fixing;
        mCurrentKey = event->key();
      }
      mCurrentModifiers = event->modifiers();
      updateKeySequence(mCurrentKey, mCurrentModifiers);
      break;
    case Fixing:
      if(isValid(event->key())) {
        mCurrentKey = event->key();
        mCurrentModifiers = event->modifiers();
      } else {
        mCurrentModifiers |= event->modifiers();
      }
      updateKeySequence(mCurrentKey, mCurrentModifiers);
      break;
    default:
      break;
    }
  }

  void KeySequenceEdit::keyReleaseEvent(QKeyEvent* event) {
    switch(mState) {
    case Init:
      mCurrentKey = 0;
      mCurrentModifiers = 0;
      updateKeySequence(mCurrentKey, mCurrentModifiers);
      break;
    case Constructing:
      mCurrentKey = 0;
      mCurrentModifiers = event->modifiers();
      updateKeySequence(mCurrentKey, mCurrentModifiers);
      break;
    case Fixing:
      if(isValid(event->key())) {
        mKeySequence = keysToKeySequence(mCurrentKey, mCurrentModifiers);
        updateKeySequence(mKeySequence);
        mState = Init;
      }
      break;
    default:
      break;
    }
  }

  void KeySequenceEdit::focusOutEvent(QFocusEvent* event) {
    if(event->lostFocus() && mState == Fixing) {
      mKeySequence = keysToKeySequence(mCurrentKey, mCurrentModifiers);
      updateKeySequence(mKeySequence);
    }
    mState = Init;
    QLineEdit::focusOutEvent(event);
  }

  QKeySequence KeySequenceEdit::keysToKeySequence(int key, int modifiers) {
    return QKeySequence(key + (modifiers & ~Qt::KeypadModifier));
  }

  void KeySequenceEdit::updateKeySequence(QKeySequence keySequence) {
    setText(keySequence.toString(QKeySequence::NativeText));
  }

  void KeySequenceEdit::updateKeySequence(int key, int modifiers) {
    if(key == 0 && modifiers == 0)
      updateKeySequence(mKeySequence);
    else
      updateKeySequence(keysToKeySequence(key, modifiers));
  }

  bool KeySequenceEdit::isValid(int key) {
    switch (key) {
    case 0:
    case Qt::Key_unknown:
      return false;
    }

    return !isModifier(key);
  }

  bool KeySequenceEdit::isModifier(int key) {
    switch (key) {
    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Meta:
    case Qt::Key_Alt:
    case Qt::Key_AltGr:
    case Qt::Key_Super_L:
    case Qt::Key_Super_R:
    case Qt::Key_Menu:
      return true;
    default:
      return false;
    }
  }

} // namespace shinobi
