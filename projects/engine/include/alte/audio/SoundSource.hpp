#pragma once

#include <alte/audio/AudioResource.hpp>
#include <glm/glm.hpp>

namespace AltE {
  class SoundSource : public AudioResource {
    public:
      /**
       * @brief Enumeration of the sound source state
       *
       */
      enum Status {
        /// Sound is not playing
        Stopped,
        /// Sound is paused
        Paused,
        /// Sound is playing
        Playing
      };

      /**
       * @brief Copy constructor
       *
       * @param copy Instance to copy
       */
      SoundSource(const SoundSource &copy);

      /**
       * @brief Deconstructor
       */
      virtual ~SoundSource();

      /**
       * @brief Set the pitch of the sound
       *
       * The pitch represents the perceived fundamental frequency of a sound;
       * thus you can make a sound more acute or grave by changing its pitch. A
       * side effect of changing the pitch is to modify the playing speed of the
       * sound as well.
       * The default value for the pitch is 1.
       *
       * @param pitch New pitch to apply to the sound
       * @see get_pitch
       */
      void set_pitch(float pitch);

      /**
       * @brief Set the volume of the sound
       *
       * The volume is value between 0 (mute) and 100 (full volume).
       * The default value for the volume is 100.
       *
       * @param volume Volume of the sound
       * @see get_volume
       */
      void set_volume(float volume);

      /**
       * @brief Set the 3D position of the sound in the audio space
       *
       * Only sound with one channel (mono sound) can be spatialized.
       * The default position of a sound is (0, 0, 0).
       *
       * @param position Position of the sound in the scene
       * @see get_position
       */
      void set_position(const glm::vec3 &position);

      /**
       * @brief Make the sound's position relative to the listener or absolute
       *
       * Making a sound relative to the listener will ensure that it will always
       * be played the same way regardless of the position of the listener. The
       * can be useful for non-spatialized sounds, sounds that are produced by
       * the listener, or sounds attached to it. The default value is false
       * (position is absolute).
       *
       * @param relative True to set the position relative, false to set it
       * absolute
       *
       * @see is_relative_to_listener
       */
      void set_relative_to_listener(bool relative);

      /**
       * @brief Set the minimum distance of the sound
       *
       * The "minimum distance" of a sound is the maximum distance at which it
       * is head at its maximum volume. Further than the minimum distance, it
       * will start to fade out according to its attenuation factor. a valud of
       * 0 ("inside the head of the listener") is an invalid value and is
       * forbidden.
       * The default value ofthe minimum distance is 1.
       *
       * @param distance New minimum distance of the sound
       * @see get_min_distance, set_attenuation
       */
      void set_min_distance(float distance);

      /**
       * @brief Set the attenuation factor of the sound
       *
       *  The attenuation is a multiplicative factor which makes
       * the sound more or less loud according to its distance
       * from the listener. An attenuation of 0 will produce a
       * non-attenuated sound, i.e. its volume will always be the same
       * whether it is heard from near or from far. On the other hand,
       * an attenuation value such as 100 will make the sound fade out
       * very quickly as it gets further from the listener.
       * The default value of the attenuation is 1.
       *
       * @param attenuation New attenuation factor of the sound
       * @see get_attenuation, set_min_distance
       */
      void set_attenuation(float attenuation);

      /**
       * @brief Get the pitch of the sound
       *
       * @return The pitch of the sound
       * @see set_pitch
       */
      float get_pitch() const;

      /**
       * @brief Get the 3D position of the sound in the audio scene
       *
       * @return Position of the sound
       * @see set_position
       */
      glm::vec3 get_position() const;

      /**
       * @brief Get the minimum distance of the sound
       *
       * @return Minimum distance of the sound
       * @see set_min_distance
       */
      float get_min_distance() const;

      /**
       * @brief Get the attenuation factor of the sound
       *
       * @return Attenuation factor of the sound
       * @see set_attenuation
       */
      float get_attenuation() const;

      /**
       * @brief Get the volume of the sound
       *
       * @return Volume of the sound, in the range [0, 100]
       * @see set_volume
       */
      float get_volume() const;

      /**
       * @brief Tell whether the sound's position is relative to the listener or
       * is absolute
       *
       * @return True if the sound is relative, false if it is absolute
       * @see set_relative_to_listener
       */
      bool is_relative_to_listener() const;

      /**
       * @brief Overload of assigment operator
       *
       * @param right Instance to assign
       * @return Reference to the assigned instance
       */
      SoundSource &operator=(const SoundSource &right);

      /**
       * @brief Start or resume playing the sound source
       *
       * This function starts the source if it was stopped, resumes
       * it if it was paused, and restarts it from the beginning if it
       * was already playing.
       *
       * @see pause, stop
       */
      virtual void play() = 0;

      /**
       * @brief Stop playing the sound source
       *
       * This function stops the source if it was playing or paused,
       * and does nothing if it was already stopped.
       * It also resets the playing position (unlike pause()).
       *
       * @see play, pause
       */
      virtual void stop() = 0;

      /**
       * @brief Pause the sound source
       *
       * This function pauses the source if it was playing,
       * overthise (source already paused or stopped) it has no effect.
       *
       * @see play, stop
       */
      virtual void pause() = 0;

      /**
       * @brief Get the current status of the sound (stopped, paused, playing)
       *
       * @return Status of the sound source
       */
      virtual Status get_status() const;

    protected:
      /**
       * @brief Default constructor
       *
       * This constructor it meant to be called by derived classes only.
       */
      SoundSource();

      /// OpenAL source identifier
      unsigned int _source;
  };
} // namespace AltE