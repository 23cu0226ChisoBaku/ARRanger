//*************************************************
// 斥力引力の干渉を受けるオブジェクトのベースクラス
//*************************************************

#pragma once

#include "GameFramework/Actor.h"
#include "Public/IARMagnetizableInterface.h"

#include "MagnetizableActor.generated.h"


UCLASS(Abstract)
class AMagnetizableActor :  public AActor, 
							public IARMagnetizableInterface
{
	GENERATED_BODY()

protected:
	ARRANGER_API virtual void BeginPlay() override;
	
public:	
	ARRANGER_API AMagnetizableActor();
	ARRANGER_API virtual void Tick(float DeltaTime) override;


	/*Start IARMagnetizableInterface interface*/
	ARRANGER_API virtual void OnAttraction() override;
	ARRANGER_API virtual void OnRepulsion() override;
	ARRANGER_API virtual AActor* GetActor() const override { return (AActor*)this; }
	/*End IARMagnetizableInterface interface*/

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* _pRootComponent;		// 座標の根
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPrimitiveComponent* _pMagneticField;	// 磁場範囲

};
