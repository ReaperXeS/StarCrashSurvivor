// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

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
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnWeaponBoxOverlapBegin);
}

void AWeapon::OnWeaponBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	IgnoreActors.Add(GetOwner());
	FHitResult OutHit;

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		BoxTraceStart->GetComponentLocation(),
		BoxTraceEnd->GetComponentLocation(),
		FVector(5.f, 5.f, 5.f),
		BoxTraceStart->GetComponentRotation(),
		TraceTypeQuery1,
		false,
		IgnoreActors,
		EDrawDebugTrace::ForDuration,
		OutHit,
		true);
}

void AWeapon::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
