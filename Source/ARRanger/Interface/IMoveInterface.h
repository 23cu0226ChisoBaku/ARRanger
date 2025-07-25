//*************************************************
// 移動をするためのインターフェース
//*************************************************

#pragma once

/*
* Interface for implementing moves
*/
class ImoveInterface
{
public:
	virtual void Attack() = 0;					// 移動の処理を行う関数
	virtual bool CanAttack() const = 0;			// 移動可能かどうかを返す関数
	virtual float SetMoveSpeed() const = 0;		// 移動スピードを設定する関数
};