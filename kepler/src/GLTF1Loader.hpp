#pragma once

#include "Base.hpp"
#include <string>

namespace kepler {

    /// Class for loading GLTF files.
    class GLTF1Loader final {
    public:
        GLTF1Loader();

        /// Loads the GLTF file at the given path.
        /// @param[in] path The file path.
        explicit GLTF1Loader(const char* path);
        ~GLTF1Loader() noexcept;

        /// True if a GLTF file was loaded successfully; false otherwise.
        explicit operator bool() const;

        /// Loads a GLTF file.
        /// @param[in] path The file path.
        /// @return True if the GLTF was loaded successfully; false otherwise;
        bool load(const char* path);

        /// Loads the default scene from the given GLTF file.
        /// @param[in] path The file path.
        /// @return A reference to the newly loaded scene. Will be empty if there was an error.
        ref<Scene> loadSceneFromFile(const char* path);

        ref<Material> findMaterialById(const std::string& id);
        ref<Material> findMaterialByName(const std::string& name);

        ref<Mesh> findMeshById(const std::string& id);

        /// Clears all of the data held by this loader.
        void clear();

        /// Sets if the default material should be used even if a primitive has a material listed.
        void useDefaultMaterial(bool value);

        /// Sets if materials will be automatically loaded when loading scenes, nodes or meshes.
        /// Enabled by default.
        /// This will not affect explicitly loading materials using methods like findMaterialById().
        void setAutoLoadMaterials(bool value);

        /// Sets the aspect ratio to use when loading cameras.
        /// @param[in] aspectRatio The aspect ratio to use. Zero means use what is found in the glTF file.
        void setCameraAspectRatio(float aspectRatio);

        /// Prints the time spent loading GLTF files so far.
        static void printTotalTime();

    public:
        GLTF1Loader(const GLTF1Loader&) = delete;
        GLTF1Loader& operator=(const GLTF1Loader&) = delete;
    private:
        class Impl;
        std::unique_ptr<Impl> _impl;
    };
}
