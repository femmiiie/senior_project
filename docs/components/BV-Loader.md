# BV Loader

Utility for loading patch data from `.bv` files.

## Public Functions

### `namespace BVLoader`

Free functions for parsing `.bv` files into [`PatchData`](/components/PatchData#struct-patchdata).

**Functions**

- [`PatchData`](/components/PatchData#struct-patchdata) ` Load(const std::string& filepath,` [`Status`](/components/PatchData#enum-status)`* status = nullptr)`: Loads all patches from the given file. Optionally writes the result status to `status`.
- [`PatchData`](/components/PatchData#struct-patchdata) ` Load(const std::string& filepath, uint32_t max_patches,` [`Status`](/components/PatchData#enum-status)`* status = nullptr)`: Loads up to `max_patches` patches from the given file. Optionally writes the result status to `status`.