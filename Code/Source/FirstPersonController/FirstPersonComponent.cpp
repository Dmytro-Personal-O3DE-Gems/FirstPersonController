
#include "FirstPersonComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace FirstPersonController
{
    AZ_COMPONENT_IMPL(FirstPersonComponent, "FirstPersonComponent", "{85060FF7-A799-4D65-9591-8FA8DD86EDEF}");

    void FirstPersonComponent::Activate()
    {
        FirstPersonRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TickBus::Handler::BusConnect();
        Physics::CharacterNotificationBus::Handler::BusConnect(GetEntityId());

        // Inputs
        StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(
            StartingPointInput::InputEventNotificationId("Forward"));
        StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(
            StartingPointInput::InputEventNotificationId("Back"));
        StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(
            StartingPointInput::InputEventNotificationId("Left"));
        StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(
            StartingPointInput::InputEventNotificationId("Right"));
        StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(
            StartingPointInput::InputEventNotificationId("Sprint"));
            StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(
            StartingPointInput::InputEventNotificationId("Crouch"));
        StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(
            StartingPointInput::InputEventNotificationId("Jump"));

        StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(
            StartingPointInput::InputEventNotificationId("Yaw"));
        StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(
            StartingPointInput::InputEventNotificationId("Pitch"));
        // ~Inputs

        // EntityBus::Handler
        // Если камера уже активна, то сразу получаем её локальный трансформ
        if (m_cameraEntityId.IsValid())
    {
        AZ::Entity* cameraEntity = nullptr;
        AZ::ComponentApplicationBus::BroadcastResult(cameraEntity, &AZ::ComponentApplicationBus::Events::FindEntity, m_cameraEntityId);

        if (cameraEntity && cameraEntity->GetState() == AZ::Entity::State::Active)
        {
            AZ::TransformBus::EventResult(m_tInitialCameraTransform, m_cameraEntityId, &AZ::TransformBus::Events::GetLocalTranslation);
            m_tCameraBaseLocalTranslation = m_tInitialCameraTransform;
        }
        else
        {
            AZ::EntityBus::Handler::BusConnect(m_cameraEntityId);
        }
    }
    }

    void FirstPersonComponent::Deactivate()
    {
        FirstPersonRequestBus::Handler::BusDisconnect(GetEntityId());
        Physics::CharacterNotificationBus::Handler::BusDisconnect();
        AZ::TickBus::Handler::BusDisconnect();

        AZ::EntityBus::Handler::BusDisconnect();

        StartingPointInput::InputEventNotificationBus::MultiHandler::BusDisconnect();
    }

    void FirstPersonComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<FirstPersonComponent, AZ::Component>()
                ->Version(1)
                ->Field("WalkSpeed", &FirstPersonComponent::m_fWalkSpeed)
                ->Field("BackwardSpeedMultiplier", &FirstPersonComponent::m_fBackwardSpeedMultiplier)
                ->Field("MovementAcceleration", &FirstPersonComponent::m_fAcceleration)
                // Sprint
                ->Field("CanSprint", &FirstPersonComponent::m_bCanSprint)
                ->Field("SprintSpeed", &FirstPersonComponent::m_fSprintSpeed)
                // Crouch
                ->Field("CanCrouch", &FirstPersonComponent::m_bCanCrouch)
                ->Field("CrouchSpeed", &FirstPersonComponent::m_fCrouchSpeed)
                // Jump
                ->Field("CanJump", &FirstPersonComponent::m_bCanJump)
                ->Field("JumpSpeed", &FirstPersonComponent::m_fJumpSpeed)

                ->Field("CameraEntity", &FirstPersonComponent::m_cameraEntityId)
                ->Field("MouseSensitivityX", &FirstPersonComponent::m_fMouseSensitivityX)
                ->Field("MouseSensitivityY", &FirstPersonComponent::m_fMouseSensitivityY)

                // Camera Bob
                ->Field("EnableCameraBob", &FirstPersonComponent::m_bEnableCameraBob)
                ->Field("BobFrequency", &FirstPersonComponent::m_fBobFrequency)
                ->Field("BobVerticalAmplitude", &FirstPersonComponent::m_fBobVerticalAmplitude)
                ->Field("BobHorizontalAmplitude", &FirstPersonComponent::m_fBobHorizontalAmplitude)
                ->Field("BobSmoothSpeed", &FirstPersonComponent::m_fBobSmoothSpeed)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<FirstPersonComponent>("FirstPersonComponent", "[Description of functionality provided by this component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Components")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))


                // --- Группа "Camera & Look" ---
                ->ClassElement(AZ::Edit::ClassElements::Group, "Camera & Look")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_cameraEntityId,
                    "Camera Entity", "Дочерняя сущность камеры — на неё применяется вертикальный поворот (pitch)")
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_fMouseSensitivityX,
                    "Sensitivity X", "Чувствительность мыши по горизонтали")
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_fMouseSensitivityY,
                    "Sensitivity Y", "Чувствительность мыши по вертикали")

                // --- Группа "Camera Bob" ---
                ->ClassElement(AZ::Edit::ClassElements::Group, "Camera Bob")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_bEnableCameraBob,
                    "Enable Camera Bob", "Включить покачивание камеры при ходьбе")
                    ->Attribute(AZ::Edit::Attributes::ReadOnly, &FirstPersonComponent::isCameraBobReadOnly)
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_fBobFrequency,
                    "Bob Frequency", "Скорость покачивания относительно пройденного расстояния")
                    ->Attribute(AZ::Edit::Attributes::ReadOnly, &FirstPersonComponent::isCameraBobReadOnly)
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_fBobVerticalAmplitude,
                    "Vertical Amplitude", "Размах покачивания вверх-вниз, метры")
                    ->Attribute(AZ::Edit::Attributes::ReadOnly, &FirstPersonComponent::isCameraBobReadOnly)
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_fBobHorizontalAmplitude,
                    "Horizontal Amplitude", "Размах покачивания влево-вправо, метры")
                    ->Attribute(AZ::Edit::Attributes::ReadOnly, &FirstPersonComponent::isCameraBobReadOnly)
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_fBobSmoothSpeed,
                    "Smoothing Speed", "Как быстро покачивание гасится при остановке")
                    ->Attribute(AZ::Edit::Attributes::ReadOnly, &FirstPersonComponent::isCameraBobReadOnly)

                // --- Группа "Walk" ---
                ->ClassElement(AZ::Edit::ClassElements::Group, "Walk")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_fWalkSpeed,
                    "Walk Speed", "Скорость ходьбы, м/с")
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_fBackwardSpeedMultiplier,
                    "Backward Speed Multiplier", "Множитель скорости при движении назад")
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_fAcceleration,
                    "Acceleration Movement", "Acceleration Movement")

                // --- Группа "Sprint" ---
                ->ClassElement(AZ::Edit::ClassElements::Group, "Sprint")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_bCanSprint,
                    "Can Sprint", "Разрешён ли спринт вообще")
                    ->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::AttributesAndValues)
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_fSprintSpeed,
                    "Sprint Speed", "Скорость бега, м/с")
                    ->Attribute(AZ::Edit::Attributes::ReadOnly, &FirstPersonComponent::IsSprintReadOnly)

                // --- Группа "Crouch" ---
                ->ClassElement(AZ::Edit::ClassElements::Group, "Crouch")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_bCanCrouch,
                    "Can Crouch", "Разрешено ли приседать")
                    ->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::AttributesAndValues)
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_fCrouchSpeed,
                    "Crouch Speed", "Скорость приседания, м/с")
                    ->Attribute(AZ::Edit::Attributes::ReadOnly, &FirstPersonComponent::isCrouchReadOnly)

                // --- Группа "Jump" ---
                ->ClassElement(AZ::Edit::ClassElements::Group, "Jump")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_bCanJump,
                    "Can Jump", "Разрешено ли прыгать")
                    ->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::AttributesAndValues)
                ->DataElement(AZ::Edit::UIHandlers::Default, &FirstPersonComponent::m_fJumpSpeed,
                    "Jump Speed", "Скорость прыжка, м/с")
                    ->Attribute(AZ::Edit::Attributes::ReadOnly, &FirstPersonComponent::isJumpReadOnly)


                ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<FirstPersonComponent>("FirstPerson Component Group")
                ->Attribute(AZ::Script::Attributes::Category, "FirstPersonController Gem Group")
                ;
        }
    }

    void FirstPersonComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("FirstPersonComponentService"));
    }

    void FirstPersonComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("InputService"));
    }

    void FirstPersonComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("InputConfigurationService"));
        required.push_back(AZ_CRC_CE("PhysicsCharacterControllerService"));
        required.push_back(AZ_CRC_CE("PhysicsCharacterGameplayService"));
    }

    void FirstPersonComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    // Input methods
    AZ::Vector3 FirstPersonComponent::CalculateLocalMoveDirection() const {
        AZ::Vector3 direction(m_moveRight, m_moveForward, 0.0f);

        if (!direction.IsZero()) { direction.Normalize(); }

        return direction;
    }

    float FirstPersonComponent::CalculateForwardSpeed(float forwardAxis) const {
        float resultSpeed = CalculateBaseSpeed();

        if (!m_bIsCrouching && m_bIsSprinting)
            resultSpeed = m_fSprintSpeed;

        if (forwardAxis < 0.0f)
            resultSpeed *= m_fBackwardSpeedMultiplier;

        return resultSpeed;
    }

    float FirstPersonComponent::CalculateBaseSpeed() const {
        return m_bIsCrouching ? m_fCrouchSpeed : m_fWalkSpeed;
    }

    void FirstPersonComponent::ApplyMovement(const AZ::Vector3& worldVelocity) {
        Physics::CharacterRequestBus::Event(
            GetEntityId(),
            &Physics::CharacterRequestBus::Events::AddVelocityForTick,
            worldVelocity
        );
    }

    void FirstPersonComponent::OnEntityActivated([[maybe_unused]] const AZ::EntityId& entityId)
    {
        AZ::EntityBus::Handler::BusDisconnect(); // тоже разовое событие

        AZ::TransformBus::EventResult(
            m_tInitialCameraTransform, m_cameraEntityId, &AZ::TransformBus::Events::GetLocalTranslation);

        m_tCameraBaseLocalTranslation = m_tInitialCameraTransform;
    }

    void FirstPersonComponent::OnCharacterActivated([[maybe_unused]] const AZ::EntityId& entityId)
    {
        Physics::CharacterNotificationBus::Handler::BusDisconnect();

        PhysX::CharacterControllerRequestBus::EventResult(
            m_fInitialHeight, GetEntityId(), &PhysX::CharacterControllerRequests::GetHeight);
        PhysX::CharacterControllerRequestBus::EventResult(
            m_fCapsuleRadius, GetEntityId(), &PhysX::CharacterControllerRequests::GetRadius);
        AZ::TransformBus::EventResult(
            m_tInitialCameraTransform, m_cameraEntityId, &AZ::TransformBus::Events::GetLocalTranslation);

        // не даём m_fHeightOfCrouch физически нарушить ограничение капсулы,
        // каким бы значением его ни выставили в инспекторе
        const float minValidHeight = 2.0f * m_fCapsuleRadius + 0.001f;
        m_tCameraBaseLocalTranslation = m_tInitialCameraTransform;

        m_fHeightOfCrouch = AZStd::max(m_fHeightOfCrouch, minValidHeight);
    }

    void FirstPersonComponent::OnCrouching(bool bIsCrouching)
    {
        if (bIsCrouching)
        {
            PhysX::CharacterControllerRequestBus::Event(GetEntityId(), &PhysX::CharacterControllerRequests::Resize, m_fHeightOfCrouch);
            m_tCameraBaseLocalTranslation = AZ::Vector3(
                m_tInitialCameraTransform.GetX(), m_tInitialCameraTransform.GetY(), m_fCrouchCameraHeight);
        }
        else
        {
            PhysX::CharacterControllerRequestBus::Event(GetEntityId(), &PhysX::CharacterControllerRequests::Resize, m_fInitialHeight);
            m_tCameraBaseLocalTranslation = m_tInitialCameraTransform;
        }
    }

    bool FirstPersonComponent::CanStandUp() const
    {
        auto* systemInterface = AZ::Interface<AzPhysics::SystemInterface>::Get();
        if (!systemInterface) { return true; }

        const AzPhysics::SceneHandle sceneHandle = systemInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
        AzPhysics::Scene* scene = systemInterface->GetScene(sceneHandle);
        if (!scene) { return true; }

        AZ::Transform worldTransform = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(worldTransform, GetEntityId(), &AZ::TransformBus::Events::GetWorldTM);

        const AZ::Vector3 castStart = worldTransform.GetTranslation() + AZ::Vector3(0.0f, 0.0f, m_fHeightOfCrouch - m_fCapsuleRadius);

        // запас на Contact Offset PhysX Character Controller — чтобы не вставать впритык
        constexpr float kStandUpSafetyMargin = 0.5f; // подбери опытным путём под свой Contact Offset
        const float castDistance = (m_fInitialHeight - m_fHeightOfCrouch) + kStandUpSafetyMargin;

        AzPhysics::ShapeCastRequest request = AzPhysics::ShapeCastRequestHelpers::CreateSphereCastRequest(
            m_fCapsuleRadius,
            AZ::Transform::CreateTranslation(castStart),
            AZ::Vector3(0.0f, 0.0f, 1.0f),
            castDistance,
            AzPhysics::SceneQuery::QueryType::StaticAndDynamic,
            AzPhysics::CollisionGroup::All,
            nullptr);

        request.m_reportMultipleHits = true;

        const AzPhysics::SceneQueryHits hits = scene->QueryScene(&request);

        const AZ::EntityId selfEntityId = GetEntityId();
        for (const AzPhysics::SceneQueryHit& hit : hits.m_hits)
        {
            if (hit.m_entityId != selfEntityId)
            {
                return false;
            }
        }
        return true;
    }

    void FirstPersonComponent::CheckGrounded()
    {
        auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        if (!sceneInterface) { m_bIsGrounded = false; return; }

        AZ::Transform worldTransform = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(worldTransform, GetEntityId(), &AZ::TransformBus::Events::GetWorldTM);

        // старт — центр нижней полусферы капсулы, та же геометрия, что и в CanStandUp(), только зеркально вниз
        const AZ::Vector3 castStart = worldTransform.GetTranslation() + AZ::Vector3(0.0f, 0.0f, m_fCapsuleRadius);

        AzPhysics::ShapeCastRequest request = AzPhysics::ShapeCastRequestHelpers::CreateSphereCastRequest(
            m_fCapsuleRadius,
            AZ::Transform::CreateTranslation(castStart),
            AZ::Vector3(0.0f, 0.0f, -1.0f), // вниз
            m_fGroundedSphereCastOffset,
            AzPhysics::SceneQuery::QueryType::StaticAndDynamic,
            AzPhysics::CollisionGroup::All,
            nullptr);

        request.m_reportMultipleHits = true;

        const AzPhysics::SceneHandle sceneHandle = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
        const AzPhysics::SceneQueryHits hits = sceneInterface->QueryScene(sceneHandle, &request);

        const AZ::EntityId selfEntityId = GetEntityId();
        m_bIsGrounded = false;
        for (const AzPhysics::SceneQueryHit& hit : hits.m_hits)
        {
            if (hit.m_entityId != selfEntityId)
            {
                m_bIsGrounded = true;
                break;
            }
        }
    }

    bool FirstPersonComponent::CheckHeadHit() const
    {
        auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        if (!sceneInterface) { return false; }

        AZ::Transform worldTransform = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(worldTransform, GetEntityId(), &AZ::TransformBus::Events::GetWorldTM);

        // берём РЕАЛЬНУЮ текущую высоту капсулы у контроллера, а не свой флаг —
        // это надёжнее во время самого перехода между приседом и стоя
        float currentCapsuleHeight = m_fInitialHeight;
        PhysX::CharacterControllerRequestBus::EventResult(
            currentCapsuleHeight, GetEntityId(), &PhysX::CharacterControllerRequestBus::Events::GetHeight);

        const AZ::Vector3 castStart = worldTransform.GetTranslation() + AZ::Vector3(0.0f, 0.0f, currentCapsuleHeight - m_fCapsuleRadius);

        AzPhysics::ShapeCastRequest request = AzPhysics::ShapeCastRequestHelpers::CreateSphereCastRequest(
            m_fCapsuleRadius,
            AZ::Transform::CreateTranslation(castStart),
            AZ::Vector3(0.0f, 0.0f, 1.0f),
            m_fHeadHitCheckDistance,
            AzPhysics::SceneQuery::QueryType::StaticAndDynamic,
            AzPhysics::CollisionGroup::All,
            nullptr);

        request.m_reportMultipleHits = true;

        const AzPhysics::SceneHandle sceneHandle = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
        const AzPhysics::SceneQueryHits hits = sceneInterface->QueryScene(sceneHandle, &request);

        const AZ::EntityId selfEntityId = GetEntityId();
        for (const AzPhysics::SceneQueryHit& hit : hits.m_hits)
        {
            if (hit.m_entityId != selfEntityId)
            {
                return true;
            }
        }
        return false;
    }

    void FirstPersonComponent::UpdateVerticalVelocity(float deltaTime)
    {
        CheckGrounded();
        if (m_bIsGrounded && m_fVerticalVelocity <= 0.0f)
        {
            m_fVerticalVelocity = 0.0f;
        }
        m_fVerticalVelocity += m_fGravity * deltaTime;

        // если ударились головой во время подъёма — сразу же ставим "апогей", а не ждём пока это сделает гравитация сама
        if (m_fVerticalVelocity > 0.0f && CheckHeadHit())
        {
            m_fVerticalVelocity = 0.0f;
        }
    }

    void FirstPersonComponent::UpdateHorizontalVelocity(float deltaTime)
    {
        const AZ::Vector3 localDirection = CalculateLocalMoveDirection();
        const float forwardSpeed = CalculateForwardSpeed(localDirection.GetY());
        const float strafeSpeed = CalculateBaseSpeed();

        AZ::Transform worldTransform = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(worldTransform, GetEntityId(), &AZ::TransformBus::Events::GetWorldTM);

        const AZ::Vector3 right   = worldTransform.GetBasisX();
        const AZ::Vector3 forward = worldTransform.GetBasisY();

        const AZ::Vector3 targetVelocity =
            (right * localDirection.GetX() * strafeSpeed) +      // стрейф — фиксированная скорость
            (forward * localDirection.GetY() * forwardSpeed);    // вперёд/назад — переменная

        const float t = AZStd::clamp(m_fAcceleration * deltaTime, 0.0f, 1.0f);
        m_prevTargetVelocity = m_prevTargetVelocity.Lerp(targetVelocity, t);
    }

    void FirstPersonComponent::ApplyMouseLook()
    {
        // Копим АБСОЛЮТНЫЕ углы, а не крутим кватернион инкрементально —
        // так не накапливается ошибка округления за долгую игровую сессию
        m_fCurrentYaw += -m_Yaw * m_fMouseSensitivityX;
        m_fCurrentPitch = AZStd::clamp(
            m_fCurrentPitch - m_Pitch * m_fMouseSensitivityY
            , -m_fMaxPitchDegrees, m_fMaxPitchDegrees
        );

        // Yaw — вся сущность игрока, вокруг мировой вертикали (Z)
        AZ::Transform worldTransform = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(worldTransform, GetEntityId(), &AZ::TransformBus::Events::GetWorldTM);
        worldTransform.SetRotation(AZ::Quaternion::CreateRotationZ(AZ::DegToRad(m_fCurrentYaw)));
        AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldTM, worldTransform);

        // Pitch — только камера, вокруг её собственной локальной горизонтали (X)
        if (m_cameraEntityId.IsValid())
        {
            AZ::Transform cameraLocalTransform = AZ::Transform::CreateIdentity();
            AZ::TransformBus::EventResult(cameraLocalTransform, m_cameraEntityId, &AZ::TransformBus::Events::GetLocalTM);
            cameraLocalTransform.SetRotation(AZ::Quaternion::CreateRotationX(AZ::DegToRad(m_fCurrentPitch)));
            AZ::TransformBus::Event(m_cameraEntityId, &AZ::TransformBus::Events::SetLocalTM, cameraLocalTransform);
        }

        m_Yaw = 0.0f;
        m_Pitch = 0.0f;
    }

    void FirstPersonComponent::UpdateCameraBob(float deltaTime)
    {
        AZ::Vector3 targetOffset = AZ::Vector3::CreateZero();

        if (m_bEnableCameraBob)
        {
            const float horizontalSpeed = m_prevTargetVelocity.GetLength(); // уже посчитано в UpdateHorizontalVelocity

            if (m_bIsGrounded && horizontalSpeed > 0.1f)
            {
                m_fBobPhase += horizontalSpeed * m_fBobFrequency * deltaTime;

                const float verticalOffset = sinf(2.0f * m_fBobPhase) * m_fBobVerticalAmplitude;
                const float horizontalOffset = sinf(m_fBobPhase) * m_fBobHorizontalAmplitude;

                targetOffset = AZ::Vector3(horizontalOffset, 0.0f, verticalOffset);
            }
            // если стоим на месте / в воздухе — targetOffset остаётся нулевым,
            // а фаза не продвигается, чтобы следующий шаг начинался с той же точки цикла
        }

        const float t = AZStd::clamp(m_fBobSmoothSpeed * deltaTime, 0.0f, 1.0f);
        m_currentBobOffset = m_currentBobOffset.Lerp(targetOffset, t);

        AZ::TransformBus::Event(
            m_cameraEntityId, &AZ::TransformBus::Events::SetLocalTranslation, m_tCameraBaseLocalTranslation + m_currentBobOffset);
    }
    // ~Input methods

    void FirstPersonComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time) {

        if (m_bWantsToStand && CanStandUp())
        {
            m_bIsCrouching = false;
            m_bWantsToStand = false;
            OnCrouching(m_bIsCrouching);
        }

        // Jump / Gravity
        UpdateVerticalVelocity(deltaTime);
        UpdateHorizontalVelocity(deltaTime);

        // Camera
        ApplyMouseLook();
        UpdateCameraBob(deltaTime);

        const AZ::Vector3 fullVelocity = m_prevTargetVelocity + AZ::Vector3(0.0f, 0.0f, m_fVerticalVelocity);
        ApplyMovement(fullVelocity);
    }

    // Inputs
    void FirstPersonComponent::OnPressed(float value) {
        const auto* currentId = StartingPointInput::InputEventNotificationBus::GetCurrentBusId();
        if (*currentId == SprintEventId && m_bCanSprint && !m_bIsCrouching)
        {
            m_bIsSprinting = true;
            return;
        }
        else if(*currentId == CrouchEventId && m_bCanCrouch)
        {
            m_bIsCrouching = true;
            OnCrouching(m_bIsCrouching);
            return;
        }
        else if (*currentId == JumpEventId && m_bCanJump && !m_bIsCrouching)
        {
            if (m_bIsGrounded)
            {
                m_fVerticalVelocity = m_fJumpInitialVelocity;
            }
            return;
        }

        OnHeld(value);
    }

    void FirstPersonComponent::OnHeld(float value) {
        const auto* currentId = StartingPointInput::InputEventNotificationBus::GetCurrentBusId();

        if (*currentId == ForwardEventId)      { m_moveForward = value; }
        else if (*currentId == BackEventId)    { m_moveForward = -value; }
        else if (*currentId == RightEventId)   { m_moveRight = value; }
        else if (*currentId == LeftEventId)    { m_moveRight = -value; }
        else if (*currentId == YawEventId) { m_Yaw = value; }
        else if (*currentId == PitchEventId)    { m_Pitch = value; }
    }

    void FirstPersonComponent::OnReleased([[maybe_unused]] float value) {
        const auto* currentId = StartingPointInput::InputEventNotificationBus::GetCurrentBusId();

        if (*currentId == SprintEventId && m_bCanSprint)
        {
            m_bIsSprinting = false;
            return;
        }

        if (*currentId == CrouchEventId && m_bCanCrouch)
        {
            m_bWantsToStand = true;
            if (CanStandUp())
            {
                m_bIsCrouching = false;
                m_bWantsToStand = false;
                OnCrouching(m_bIsCrouching);
            }
            // иначе остаёмся присевшими — попробуем ещё раз в OnTick
            return;
        }

        if (*currentId == StartingPointInput::InputEventNotificationId("Forward") ||
            *currentId == StartingPointInput::InputEventNotificationId("Back"))
        {
            m_moveForward = 0.0f;
        }
        else if (*currentId == StartingPointInput::InputEventNotificationId("Left") ||
            *currentId == StartingPointInput::InputEventNotificationId("Right"))
        {
            m_moveRight = 0.0f;
        }
    }
    // ~Inputs
}
