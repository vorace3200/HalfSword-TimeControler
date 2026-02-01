#pragma once
#include <cstdint>
#include <cstddef>

namespace HalfSword::SDK {

    struct FVector {
        float X, Y, Z;
    };

    struct FRotator {
        float Pitch, Yaw, Roll;
    };

    template<typename T>
    struct TArray {
        T* Data;
        int32_t Count;
        int32_t Max;
    };

    class UObject {
    public:
        void** VTable;
        int32_t ObjectFlags;
        int32_t InternalIndex;
        class UClass* ClassPrivate;
        uint8_t pad_0018[0x10];
    };
    static_assert(sizeof(UObject) == 0x28);

    class USceneComponent : public UObject {
    public:
        uint8_t pad_0028[0x100];
        FVector RelativeLocation;
    };
    static_assert(offsetof(USceneComponent, RelativeLocation) == 0x128);

    class AActor : public UObject {
    public:
        uint8_t pad_0028[0x178];
        USceneComponent* RootComponent;
    };
    static_assert(offsetof(AActor, RootComponent) == 0x1A0);

    class ULevel : public UObject {
    public:
        uint8_t pad_0028[0x78];
        TArray<AActor*> Actors;
    };
    static_assert(offsetof(ULevel, Actors) == 0xA0);

    class AGameModeBase : public AActor {
    public:
        uint8_t pad_01A8[0x180];
    };
    static_assert(sizeof(AGameModeBase) == 0x328);

    class ABP_HalfSwordGameMode_C : public AGameModeBase {
    public:
        uint8_t pad_0328[0x80];
        bool All_Enemies_Dead;
        uint8_t pad_03A9[0x27];
        int32_t Player_Rank;
        uint8_t pad_03D4[0x4];
        int32_t Current_Points;
        uint8_t pad_03DC[0x8];
        int32_t Ranks_Unlocked;
        uint8_t pad_03E8[0x3];
        bool Free_Armor;
    };
    static_assert(offsetof(ABP_HalfSwordGameMode_C, All_Enemies_Dead) == 0x3A8);
    static_assert(offsetof(ABP_HalfSwordGameMode_C, Player_Rank) == 0x3D0);
    static_assert(offsetof(ABP_HalfSwordGameMode_C, Current_Points) == 0x3D8);
    static_assert(offsetof(ABP_HalfSwordGameMode_C, Ranks_Unlocked) == 0x3E4);
    static_assert(offsetof(ABP_HalfSwordGameMode_C, Free_Armor) == 0x3EB);

    class UWorld : public UObject {
    public:
        uint8_t pad_0028[0x130];
        AGameModeBase* AuthorityGameMode;
        uint8_t pad_0160[0x18];
        TArray<ULevel*> Levels;
    };
    static_assert(offsetof(UWorld, Levels) == 0x178);

    class UCharacterMovementComponent : public UObject {
    public:
        uint8_t pad_0028[0x148];
        float Gravity_Scale;
        uint8_t pad_0174[0x4];
        float Jump_Z_Velocity;
        uint8_t pad_017C[0x88];
        float Ground_Friction;
        uint8_t pad_0208[0x40];
        float Max_Walk_Speed;
        float Max_Walk_Speed_Crouched;
        float Max_Swim_Speed;
        float Max_Fly_Speed;
        float Max_Custom_Movement_Speed;
        float Max_Acceleration;
        uint8_t pad_0260[0x70];
        float Mass;
    };
    static_assert(offsetof(UCharacterMovementComponent, Gravity_Scale) == 0x170);
    static_assert(offsetof(UCharacterMovementComponent, Jump_Z_Velocity) == 0x178);
    static_assert(offsetof(UCharacterMovementComponent, Max_Walk_Speed) == 0x248);
    static_assert(offsetof(UCharacterMovementComponent, Mass) == 0x2D0);

    class APawn : public AActor {
    public:
        uint8_t pad_01A8[0x170];
    };
    static_assert(sizeof(APawn) == 0x318);

    class ACharacter : public APawn {
    public:
        class USkeletalMeshComponent* Mesh;
        class UCharacterMovementComponent* CharacterMovement;
    };
    static_assert(offsetof(ACharacter, Mesh) == 0x318);
    static_assert(offsetof(ACharacter, CharacterMovement) == 0x320);

    class AWillie_BP_C : public ACharacter {
    public:
        uint8_t pad_0328[0x358];
        class UGroomComponent* Hair;
        uint8_t pad_0688[0xC90];
        double Health;
        uint8_t pad_1320[0x1A8];
        double Muscle_Power;
        uint8_t pad_14D0[0x10];
        double Pain;
        uint8_t pad_14E8[0x118];
        double Head_Health;
        double Body_Upper_Health;
        double Body_Lower_Health;
        double Arm_R_Health;
        double Arm_L_Health;
        double Leg_R_Health;
        double Leg_L_Health;
        uint8_t pad_1638[0x470];
        double Neck_Health;
        uint8_t pad_1AB0[0x78];
        int32_t Team_Int;
        uint8_t pad_1B2C[0x394];
        double R_Fisting_Rate;
        double L_Fisting_Rate;
        uint8_t pad_1ED0[0x3AA];
        bool Invulnerable;
        uint8_t pad_227B[0x185];
        double Damage_Rate_Additional;
        uint8_t pad_2408[0x8];
        double Health_Threshold_For_Dismemberment;
        
        bool NPC_Dualist;
        uint8_t pad_2419[0x53F];
        
        double Stamina;
        uint8_t pad_2960[0xB2C];
        float Kick_Rate_R;
        uint8_t pad_3490[0x4];
        float Kick_Rate_L;
        uint8_t pad_3498[0x7D];
        bool Kick_Cooldown;
        uint8_t pad_3516[0x392];
        bool Arm_R_Broken;
        bool Arm_L_Broken;
        bool Leg_R_Broken;
        bool Leg_L_Broken;
        uint8_t pad_38AC[0x90C];
        bool Back_Broken;
        uint8_t pad_41B9[0x28];
        bool Head_Broken;
    };
    


}
