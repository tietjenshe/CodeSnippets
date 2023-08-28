#include "Turret.h"

#include <glm/gtx/vector_angle.hpp>

#include "engine/rendering/ReflectiveMaterial.h"
#include "Bullet.h"

namespace spacedust {

	Turret::Turret(engine::guid_t entityID, engine::EntityComponentManager& entityManager,
		engine::Transform& shipTransform, engine::Transform* parent, 
		glm::vec3 position, glm::quat rotation, glm::vec3 scale)
		:
		// Due to lack of serialisation in the engine a large number of parameters have to be specified here
		engine::Entity(entityID, entityManager),
		m_transform(addComponent<engine::Transform>(parent, position, rotation, scale)),
		m_renderer(addComponent<engine::Renderer>(m_transform, 
			std::make_shared<engine::rendering::ReflectiveMaterial>("textures/Turret-Diffuse.png"),
			engine::rendering::Mesh::getInstance("gltf/turret.glb"))),
		m_rigidbody(addComponent<engine::Rigidbody>(m_transform, rp3d::BodyType::STATIC)),
		m_firingAudio(addComponent<engine::Audio>("audio/shot_enemy.mp3", 0.04f, false)),
		m_target(shipTransform)
	{
		m_rigidbody.addBoxCollider();
		m_rigidbody.onEnterCollisionCallback = [&](engine::Physicsbody* otherBody) { onCollisionEntered(otherBody); };

		// Adds this update method to the general update event. Pushes also to a list that unsubscribes all methods from their respective events upon destruction
		m_unsubscribeFunctions.push_back(entityManager.getGameApplication().onUpdate.addListener([&]() { update(); }));
	}

	void Turret::update() {
		engine::Physicsbody* raycastHit = m_rigidbody.raycast(m_transform.getWorldPosition(), m_target.m_position, m_range);
		if (raycastHit != nullptr && raycastHit->compareTag("Player")) {
			rotateTowardsTarget();
			shoot();
		}

		if (m_firingCooldown > 0) {
			m_firingCooldown -= engine::GameApplication::getDeltaTime() * m_firingRate;
		}
	}

	void Turret::rotateTowardsTarget() {
		glm::vec3 targetDirection = m_target.m_position - m_transform.getWorldPosition();
		glm::quat targetRotation = glm::quatLookAt(glm::normalize(-targetDirection), glm::normalize(m_transform.getUpVector()));

		float angle = glm::degrees(glm::angle(glm::normalize(m_transform.getForwardVector()), glm::normalize(targetDirection)));

		float progress = m_turningRate * engine::GameApplication::getDeltaTime() / angle;
		progress = glm::clamp(progress, 0.0f, 1.0f);
		
		m_transform.m_rotation = glm::slerp(m_transform.m_rotation, targetRotation, progress);
	}

	void Turret::shoot() {
		if (m_firingCooldown <= 0) {
			m_entityManager.createEntity<Bullet>(m_firingSpeed, m_bulletColor,
				m_transform.getWorldPosition() +
				m_transform.getForwardVector() * 6.0f, m_transform.getWorldRotation());
			m_firingCooldown = 1;
			m_firingAudio.play();
		}
	}

	void Turret::onCollisionEntered(engine::Physicsbody* otherBody) {
		if (otherBody->compareTag("Bullet")) {
			if (--m_health <= 0) { destroyTurret(); }
		}
		else if (otherBody->compareTag("Player")) {
			destroyTurret();
		}
	}

	void Turret::destroyTurret() {
		deleteEntity();
	}
}