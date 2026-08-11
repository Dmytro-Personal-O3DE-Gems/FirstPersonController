
#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <FirstPersonController/FirstPersonController/FirstPersonInterface.h>

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Component/EntityBus.h>

// Physics
#include <AzCore/Math/Vector3.h>          // AZ::Vector3
#include <AzCore/Math/Transform.h>        // AZ::Transform
#include <AzCore/Component/TransformBus.h> // AZ::TransformBus
#include <AzFramework/Physics/CharacterBus.h>

// --- AzPhysics: Сцена, Запросы и Коллизии ---
#include <AzFramework/Physics/PhysicsSystem.h>            // AzPhysics::SystemInterface — GetSceneHandle / GetScene
#include <AzFramework/Physics/PhysicsScene.h>                // AzPhysics::Scene / SceneInterface, QueryScene, DefaultPhysicsSceneName
#include <AzFramework/Physics/Common/PhysicsSceneQueries.h>   // RayCastRequest / ShapeCastRequest / SceneQueryHits / ShapeCastRequestHelpers
#include <AzFramework/Physics/Collision/CollisionGroups.h>    // CollisionGroup

#include <PhysX/CharacterControllerBus.h>

#include <StartingPointInput/InputEventNotificationBus.h>

namespace FirstPersonController
{
    class FirstPersonComponent
        : public AZ::Component
        , protected AZ::TickBus::Handler
        , public FirstPersonRequestBus::Handler
        , protected Physics::CharacterNotificationBus::Handler
        , protected StartingPointInput::InputEventNotificationBus::MultiHandler
        , protected AZ::EntityBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(FirstPersonComponent);

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        void Activate() override;
        void Deactivate() override;
        virtual void OnTick(float deltaTime, AZ::ScriptTimePoint time);
        
        // Inputs implemented methods
        void OnPressed(float value) override;
        void OnHeld(float value) override;
        void OnReleased(float value) override;

        // Physics::CharacterNotificationBus::Handler
        void OnCharacterActivated([[maybe_unused]] const AZ::EntityId& entityId);

        // EntityBus::Handler
        void OnEntityActivated(const AZ::EntityId& entityId) override;

    private:
        // Input variables

            // Movement
            float m_moveForward = 0.0f;
            float m_moveRight = 0.0f;
            float m_fWalkSpeed = 3.5f;
            float m_fBackwardSpeedMultiplier = 0.5f; // назад — вдвое медленнее, и без спринта
            AZ::Vector3 m_prevTargetVelocity { m_prevTargetVelocity.CreateZero() };
                // Sprint
                bool    m_bCanSprint = true;
                bool    m_bIsSprinting = false;
                float   m_fSprintSpeed = 6.0f;
                bool    IsSprintReadOnly() const { return !m_bCanSprint; }

                // Crouching
                bool    m_bCanCrouch = true;
                bool    m_bIsCrouching = false;
                float   m_fInitialHeight { 0.0f };
                float   m_fHeightOfCrouch { 0.3f };
                float   m_fCrouchSpeed { 1.5f };
                bool    m_bWantsToStand = false;
                bool    isCrouchReadOnly() const { return !m_bCanCrouch; }

                // Jumping
                bool    m_bCanJump = true;
                bool    isJumpReadOnly() const { return !m_bCanJump; }
                bool    m_bIsGrounded = false;
                float   m_fJumpSpeed = 5.0f;
                float   m_fGravity = -9.8f;                 // м/с², ускорение свободного падения
                float   m_fJumpInitialVelocity = 5.0f;      // м/с, стартовая вертикальная скорость прыжка
                float   m_fVerticalVelocity = 0.0f;         // персистентное состояние между тиками
                float   m_fGroundedSphereCastOffset = 0.1f; // насколько ниже ног проверяем землю
                float   m_fHeadHitCheckDistance = 0.20f;

            
            float m_fAcceleration = 10.0f;
            // ~Movement

            // Camera
            AZ::EntityId m_cameraEntityId;

            float m_Yaw = 0.0f;
            float m_Pitch = 0.0f;

            float m_fMouseSensitivityX = 0.1f;
            float m_fMouseSensitivityY = 0.1f;

            AZ::Vector3 m_tInitialCameraTransform { m_tInitialCameraTransform.CreateZero() };
            float m_fCrouchCameraHeight = 0.65f;

            float m_fCurrentYaw = 0.0f;
            float m_fCurrentPitch = 0.0f;
            static constexpr float m_fMaxPitchDegrees = 89.0f;
            // ~Camera

            // Capsule
            float m_fCapsuleRadius;

        // ~Input variables

        // Camera Bob
        bool    m_bEnableCameraBob = true;
        float   m_fBobFrequency = 6.0f;           // скорость покачивания относительно пройденного расстояния
        float   m_fBobVerticalAmplitude = 0.03f;  // м, размах вверх-вниз
        float   m_fBobHorizontalAmplitude = 0.02f;// м, размах влево-вправо
        float   m_fBobSmoothSpeed = 8.0f;         // как быстро покачивание "успокаивается" при остановке
        float   m_fBobPhase = 0.0f;               // накопленная фаза
        AZ::Vector3 m_tCameraBaseLocalTranslation { m_tCameraBaseLocalTranslation.CreateZero() }; // позиция камеры БЕЗ покачивания
        AZ::Vector3 m_currentBobOffset { m_currentBobOffset.CreateZero() };
        bool isCameraBobReadOnly() const { return !m_bEnableCameraBob; }
        // ~Camera Bob

        // Input methods

            // ~Movement
            AZ::Vector3 CalculateLocalMoveDirection() const;
            float CalculateForwardSpeed(float forwardAxis) const;
            void ApplyMovement(const AZ::Vector3& worldVelocity);

            void OnCrouching(bool bIsCrouching);

            float CalculateBaseSpeed() const;
            // ~Movement

            // Mouse Look
            void ApplyMouseLook();
            // ~Mouse Look
            void UpdateCameraBob(float deltaTime);

            // Crouching
            bool CanStandUp() const;

            // ~Crouching
            
            // Jumping
            void CheckGrounded();
            bool CheckHeadHit() const;

            void UpdateVerticalVelocity(float deltaTime);
            void UpdateHorizontalVelocity(float deltaTime);
            // ~Jumping

        // ~Input methods

        // Event Inputs ID
            // В header, приватные поля:
            // Movement
            inline static const StartingPointInput::InputEventNotificationId ForwardEventId{ "Forward" };
            inline static const StartingPointInput::InputEventNotificationId BackEventId{ "Back" };
            inline static const StartingPointInput::InputEventNotificationId LeftEventId{ "Left" };
            inline static const StartingPointInput::InputEventNotificationId RightEventId{ "Right" };
            inline static const StartingPointInput::InputEventNotificationId SprintEventId{ "Sprint" };
            inline static const StartingPointInput::InputEventNotificationId CrouchEventId{ "Crouch" };
            inline static const StartingPointInput::InputEventNotificationId JumpEventId{ "Jump" };
            // ~Movement

            // Camera
            inline static const StartingPointInput::InputEventNotificationId YawEventId{ "Yaw" };
            inline static const StartingPointInput::InputEventNotificationId PitchEventId{ "Pitch" };
            // ~Camera
        // ~Event Inputs ID
    };
} // namespace FirstPersonController
