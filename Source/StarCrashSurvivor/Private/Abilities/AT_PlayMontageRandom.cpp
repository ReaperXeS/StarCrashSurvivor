// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AT_PlayMontageRandom.h"
#include "AbilitySystemGlobals.h"

UAT_PlayMontageRandom* UAT_PlayMontageRandom::CreatePlayMontageRandom(UGameplayAbility* OwningAbility, FName TaskInstanceName, UAnimMontage* MontageToPlay, float Rate, bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale, float StartTimeSeconds)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UAT_PlayMontageRandom* MyObj = NewAbilityTask<UAT_PlayMontageRandom>(OwningAbility, TaskInstanceName);
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->Rate = Rate;
	if (MontageToPlay)
	{
		// Compute Random section
		const uint8 SectionIndex = FMath::RandRange(0, MontageToPlay->CompositeSections.Num() - 1);
		MyObj->StartSection = MontageToPlay->CompositeSections[SectionIndex].SectionName;
	}
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	MyObj->StartTimeSeconds = StartTimeSeconds;

	return MyObj;
}