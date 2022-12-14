// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(RootComponent);
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::AttachToSocket(USceneComponent* InParent, const FName SocketName)
{
	if (InParent && MeshComp)
	{
		ItemState = EItemState::EIS_Equipped;
		MeshComp->AttachToComponent(InParent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
}

bool AWeapon::ActorIsSameType(const AActor* OtherActor) const
{
	return GetOwner() != nullptr && OtherActor != nullptr && OtherActor->Tags.Num() > 0 && GetOwner()->ActorHasTag(OtherActor->Tags[0]);
}

void AWeapon::Equip(USceneComponent* InParent, FName SocketName, bool bPlaySound, AActor* NewOwner)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	AttachToSocket(InParent, SocketName);
	if (bPlaySound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}

	if (SphereComp)
	{
		SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnWeaponBoxOverlapBegin);
}

void AWeapon::OnWeaponBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor)) { return; }

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	FHitResult OutHit;
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		BoxTraceStart->GetComponentLocation(),
		BoxTraceEnd->GetComponentLocation(),
		BoxTraceExtent,
		BoxTraceStart->GetComponentRotation(),
		TraceTypeQuery1,
		false,
		IgnoreActors,
		bDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		OutHit,
		true);

	if (OutHit.GetActor())
	{
		if (const IHitInterface* HitInterface = Cast<IHitInterface>(OutHit.GetActor()); !ActorIsSameType(OutHit.GetActor()) && HitInterface)
		{
			UGameplayStatics::ApplyDamage(OutHit.GetActor(), BaseDamage, GetOwner() ? GetOwner()->GetInstigatorController() : nullptr, this, UDamageType::StaticClass());
			HitInterface->Execute_GetHit(OutHit.GetActor(), OutHit.ImpactPoint, GetOwner());
		}
		IgnoreActors.AddUnique(OutHit.GetActor());
		CreateFields(OutHit.ImpactPoint);
	}
}

void AWeapon::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
