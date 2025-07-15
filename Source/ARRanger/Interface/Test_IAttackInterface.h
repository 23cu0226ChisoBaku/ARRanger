//*************************************************
// 攻撃をするためのインターフェース
//*************************************************

#pragma once

/*
* Interface for implementing attacks
*/
class TEest_IAttackInterface
{
public:
	virtual ~TEest_IAttackInterface() = default;
 
	virtual void Attack() = 0;						// 攻撃の処理を行う関数
	virtual bool CanAttack() const = 0;				// 攻撃可能かどうかを返す関数
	virtual float GetAttackCoolTime() const = 0;	// 攻撃のクールタイムを取得する関数
};

/*
* キャラクターの側で複数の攻撃パターン分けをさせる方法で考えたインターフェース
* Attack() → Playerクラスで Attack(){Punch();Kick(); etc...}
* ただこれは、どうにかしたい.......
*/