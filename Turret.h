#pragma once
#include "engine/Entity.h"

#include "engine/components/Transform.h"
#include "engine/components/Renderer.h"
#include "engine/components/Rigidbody.h"
#include "engine/components/Audio.h"

namespace spacedust {
	/// @brief The turret entity that tracks and shoots the player ship once it enters its range and has line of sight
	class Turret : public engine::Entity {
	public:
		/// @brief Creates a turret entity
		/// @param entityID The id of the entity. Automatically passed during creation.
		/// @param entityManager The manager for all entities. Automatically passed during creation.
		/// @param shipTransform The transform the turret will be targeting
		/// @param parent The scenegraph parent of the turret.
		/// @param position The turrets position relative to the parent.
		/// @param rotation The turrets rotation relative to the parent.
		/// @param scale The turrets scale relative to the parent.
		Turret(engine::guid_t entityID, engine::EntityComponentManager& entityManager,
			engine::Transform& targetTransform,
			engine::Transform* parent = nullptr,
			glm::vec3 position = { 0, 0, 0 },
			glm::quat rotation = glm::quat(1, 0, 0, 0),
			glm::vec3 scale = { 1, 1, 1 });

	private:
		void update();
		void rotateTowardsTarget();
		void shoot();
		void destroyTurret();
		void onCollisionEntered(engine::Physicsbody* otherBody);

		// components
		engine::Transform& m_transform;
		engine::Renderer& m_renderer;
		engine::Rigidbody& m_rigidbody;
		engine::Audio& m_firingAudio;

		// stats
		int m_health = 5;					// the number of hits the turret can take

		// targeting
		engine::Transform& m_target;		// the transform that gets tracked
		float m_range = 100.0f;				// the range at which the turret can detect the target
		float m_turningSpeed = 90.0f;		// turning speed of the turret in degree per second

		// shooting
		const glm::vec4 m_bulletColor = glm::vec4(255 / 255, 90.0f / 255, 90.0f / 255, 1);
		const float m_firingVolume = 0.04f;	// the volume of the firing sound
		float m_bulletSpeed = 35.0f;		// how many units per second the bullet moves
		float m_bulletSpawnOffset = 6.0f;	// z-offset to the turret where the bullets are spawned
		float m_firingRate = 2.0f;			// bullets per seconds
		float m_firingCooldown = 0;			// goes from 0 to 1 with 0 meaning ready to fire
	};
}