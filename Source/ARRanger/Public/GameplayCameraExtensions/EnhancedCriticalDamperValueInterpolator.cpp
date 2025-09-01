// #include "GameplayCameraExtensions/EnhancedCriticalDamperValueInterpolator.h"
// #include "Core/CameraParameterReader.h"

// #include UE_INLINE_GENERATED_CPP_BY_NAME(EnhancedCriticalDamperValueInterpolator)

// namespace UE::Cameras
// {

// template<typename ValueType>
// class TEnhancedCriticalDamperValueInterpolator : public TCameraValueInterpolator<ValueType>
// {

// public:

//   using ValueTypeParam = typename TCameraValueInterpolator<ValueType>::ValueTypeParam;

//   TEnhancedCriticalDamperValueInterpolator(const UEnhancedCriticalDamperValueInterpolator* InInterpolator)
//     : TCameraValueInterpolator<ValueType>(InInterpolator)
//   {}

// protected:

// 	virtual void OnReset(ValueTypeParam OldCurrentValue, ValueTypeParam OldTargetValue) override;
// 	virtual void OnRun(const FCameraValueInterpolationParams& Params, FCameraValueInterpolationResult& OutResult) override;
// 	virtual void OnSerialize(const FCameraValueInterpolatorSerializeParams& Params, FArchive& Ar) override;

// private:

// 	UE::Cameras::FCriticalDamper Damper;
// 	bool bIsFirstFrame = true;

//   /**Use this to read  */
//   TCameraParameterReader<float> DampingFactorMultiplierReader;

// };

// template<typename ValueType>
// void TEnhancedCriticalDamperValueInterpolator<ValueType>::OnReset(ValueTypeParam OldCurrentValue, ValueTypeParam OldTargetValue)
// {
// 	const double DistanceToTarget = TCameraValueInterpolationTraits<ValueType>::Distance(this->CurrentValue, this->TargetValue);
// 	Damper.Reset(DistanceToTarget, Damper.GetX0Derivative());

//   // TODO Need research
//   if (!DampingFactorMultiplierReader.IsDriven())
//   {
//     const UEnhancedCriticalDamperValueInterpolator* EnhancedCriticalDamperInterpolator = this->template GetParametersAs<UEnhancedCriticalDamperValueInterpolator>();
//     DampingFactorMultiplierReader.Initialize(EnhancedCriticalDamperInterpolator->DampingMultiplier);
//   }
// }

// template<typename ValueType>
// void TEnhancedCriticalDamperValueInterpolator<ValueType>::OnRun(const FCameraValueInterpolationParams& Params, FCameraValueInterpolationResult& OutResult)
// {
// 	const double DistanceToTarget = TCameraValueInterpolationTraits<ValueType>::Distance(this->CurrentValue, this->TargetValue);
// 	if (bIsFirstFrame)
// 	{
// 		const UEnhancedCriticalDamperValueInterpolator* EnhancedCriticalDamperInterpolator = this->template GetParametersAs<UEnhancedCriticalDamperValueInterpolator>();
		
//     /**Multiply a multiplier to DampingFactor */
//     const float DampingMultiplierValue = DampingFactorMultiplierReader.Get(OutResult.VariableTable);
//     Damper.SetW0(EnhancedCriticalDamperInterpolator->DampingFactor * DampingMultiplierValue);
// 		Damper.Reset(DistanceToTarget, 0);
// 		bIsFirstFrame = false;
// 	}

// 	const double NextDistanceToTarget = Damper.Update(DistanceToTarget, Params.DeltaTime);
// 	const double ClosingDistance = DistanceToTarget - NextDistanceToTarget;
// 	const ValueType Direction = TCameraValueInterpolationTraits<ValueType>::Direction(this->CurrentValue, this->TargetValue);
// 	this->CurrentValue = this->CurrentValue + Direction * ClosingDistance;
// 	this->bIsFinished = ClosingDistance <= UE_DOUBLE_SMALL_NUMBER;
// }

// template<typename ValueType>
// void TEnhancedCriticalDamperValueInterpolator<ValueType>::OnSerialize(const FCameraValueInterpolatorSerializeParams& Params, FArchive& Ar)
// {
// 	Ar << Damper;
// 	Ar << bIsFirstFrame;
// }

// }  // namespace UE::Cameras

// UE_DEFINE_CAMERA_VALUE_INTERPOLATOR_GENERIC(UEnhancedCriticalDamperValueInterpolator, UE::Cameras::TEnhancedCriticalDamperValueInterpolator)
