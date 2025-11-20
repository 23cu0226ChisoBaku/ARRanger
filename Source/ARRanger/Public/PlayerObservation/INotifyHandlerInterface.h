#pragma once

#ifndef _AR_NOTIFY_HANDLER_INTERFACE_
#define _AR_NOTIFY_HANDLER_INTERFACE_

namespace ARRanger
{
  struct INotifyHandlerInterface
  {
    virtual ~INotifyHandlerInterface() = default;

    /**
     * @brief ダッシュコールバック
     */
    virtual void OnDash() = 0;

    /**
     * @brief ジャンプコールバック
     */
    virtual void OnJump() = 0;

    /**
     * @brief アタックコールバック
     */
    virtual void OnAttack() = 0;
  };
}

#endif // _AR_NOTIFY_HANDLER_INTERFACE_