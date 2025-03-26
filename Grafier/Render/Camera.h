#pragma once
#include "../Utility/DataTypes.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ent {
	namespace render {
		enum class Camera_Movement {
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT,
			UP,
			DOWN
		};

		enum class Camera_Alignment {
			AXIS,
			FREECAM
		};

		const f32 YAW = 0.0f;;
		const f32 PITCH = 0.0f;
		const f32 SPEED = 3.6f;
		const f32 SENSITIVITY = 0.1f;
		const f32 FOV = 80.0f;

		class Camera {
		public:
			f32v3 position;
			f32v3 front, up, right;
			f32v3 worldUp;
			f32 yaw, pitch;
			f32 speed;
			f32 mouseSensitivity;
			f32 fov;

			Camera_Alignment alignment;

			Camera(f32v3 position = f32v3(0.0f), f32v3 front = f32v3(0.0f, 0.0f, -1.0f), f32v3 worldUp = f32v3(0.0f, 0.0f, 1.0f),
				f32 speed = SPEED, f32 fov = FOV, f32 mouseSensitivity = SENSITIVITY, f32 yaw = YAW, f32 pitch = PITCH);

			glm::mat4 getViewMatrix();
			void processMovement(Camera_Movement direction, f32 deltaTime);
			void processMouseMovement(f32 xoffset, f32 yoffset, bool constrainPitch = true);
			void processMouseScroll(f32 yoffset);
			void updateCameraVectors();
			void lookAt(f32v3 target);
		};

	}
}

