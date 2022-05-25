#pragma once

#include <alte/export.hpp>
#include <glm/glm.hpp>
#include <stdint.h>
#include <string>
#include <vector>

class ALCdevice;
class ALCcontext;
namespace AltE {

  class ALTE_API AudioDevice {
    public:
      /**
       * @brief Constructor
       */
      AudioDevice();

      /**
       * @brief Destructor
       */
      ~AudioDevice();

      /**
       * @brief Check if an OpenAL extension is supported
       *
       * This functions automatically finds whether it is an AL or ALC
       * extension, and calls the corresponding function.
       *
       * @param extension Name of the extension to test
       * @return True if the extension is supported, false is not
       */
      static bool is_extension_supported(const std::string &extension);

      /**
       * @brief Get the OpenAL format that matches the given number of channels
       *
       * @param channel_count Number of channels
       * @return Corresponding format
       */
      static int get_format_from_channel_count(uint32_t channel_count);

      /**
       * @brief Change the volume of all the sounds and musics
       *
       * The volume is a number between 0 and 100; it is combied with the
       * individual volume of each sound / music. The default value for the
       * volume is 100 (maximum).
       *
       * @param volume New volume, in the range [0, 100]
       * @see get_volume
       */
      static void set_volume(float volume);

      /**
       * @brief Get the current value of the volume
       *
       * @return Current volume, in the range [0, 100]
       *
       * @see set_volume
       */
      static float get_volume();

      /**
       * @brief Set the position of the listener in the scene
       *
       * The default listener's position is (0, 0, 0).
       *
       * @param position New listener's position
       * @see get_position, set_direction
       */
      static void set_position(const glm::vec3 &position);

      /**
       * @brief Get the current position of the listener in the scene
       *
       * @return Listener's position
       * @see set_position
       */
      static glm::vec3 get_position();

      /**
       * @brief Set the forward vector of the listener in the scene
       *
       * The direction (also called "at vector") is the vector pointing forward
       * from the listener's perspective. Together with the up vector, it
       * defines the 3D orientation of the listener in the scene. The direction
       * vector doesn't have to be normalized.
       * The default listener's direction is (0, 0, -1).
       *
       * @param direction New listener's direction
       * @see get_direction, set_up_vector, set_position
       */
      static void set_direction(const glm::vec3 &direction);

      /**
       * @brief Get the current forward vector of the listener in the scene
       *
       * @return Listener's forward direction (not normalized)
       * @see set_direction
       */
      static glm::vec3 get_direction();

      /**
       * @brief Set the upward vector of the listener in the scene
       *
       * The up vector is the vector that points upward from the listener's
       * perspective. Together with the direction, it defines the 3D orientation
       * of the listener in the scene. The up vector doesn't have to be
       * normalized. The default listener's up vector is (0, 1, 0). It is
       * usually not necessary to change it, especially in 2D scenarios.
       *
       * @param up_vector New listener's upward vector
       * @see get_up_vector, set_direction, set_position
       */
      static void set_up_vector(const glm::vec3 &up_vector);

      /**
       * @brief Get the current upward vector of the listener in the scene
       *
       * @return Listener's upward vector (not normalized)
       * @see set_up_vector
       */
      static glm::vec3 get_up_vector();
  };
} // namespace AltE