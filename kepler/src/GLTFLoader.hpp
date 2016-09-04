#pragma once

#include "Base.hpp"
#include <string>

namespace kepler {

    /// Class for loading GLTF files.
    class GLTFLoader final {
    public:
        GLTFLoader();

        /// Loads the GLTF file at the given path.
        /// @param[in] path The file path.
        explicit GLTFLoader(const char* path);
        ~GLTFLoader() noexcept;

        /// True if a GLTF file was loaded successfully; false otherwise.
        explicit operator bool() const;

        /// Loads a GLTF file.
        /// @param[in] path The file path.
        /// @return True if the GLTFL was loaded successfully; false otherwise;
        bool load(const char* path);

        /// Loads the default scene from the given GLTF file.
        /// @param[in] path The file path.
        /// @return A reference to the newly loaded scene. Will be empty if there was an error.
        SceneRef loadSceneFromFile(const char* path);

        MaterialRef getMaterialById(const std::string& id);
        MaterialRef getMaterialByName(const std::string& name);

        MeshRef getMeshById(const std::string& id);

        /// Clears all of the data held by this loader.
        void clear();

        /// Sets if the default material should be used even if a primitive has a material listed.
        void useDefaultMaterial(bool value);

        /// Sets the aspect ratio to use when loading cameras.
        /// @param[in] aspectRatio The aspect ratio to use. Zero means use what is found in the glTF file.
        void setCameraAspectRatio(float aspectRatio);

        /// Prints the time spent loading GLTF files so far.
        static void printTotalTime();

    public:
        GLTFLoader(const GLTFLoader&) = delete;
        GLTFLoader& operator=(const GLTFLoader&) = delete;
    private:
        class Impl;
        std::unique_ptr<Impl> _impl;
    };
}
