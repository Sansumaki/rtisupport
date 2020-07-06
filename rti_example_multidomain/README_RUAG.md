# Example extension for RUAG

This example was modified to showcase an use case proposed by RUAG.

**IMPORTANT NOTE:**  
You need to copy the attached `FindRTIConnextDDS.cmake` to your Connext installation folder
  in `${NDDSHOME}/resource/cmake`.

## Description

In this example, you have a continously Recording instance and a few delayed Replay instances.
The example is configured to work with Shapes Demo, but this can be modified.

The **Recording instance** is recording **Three scenarios**:
  - Domain 50: Records Squares
  - Domain 60: Records Circles
  - Domain 70: Records Triangles

The **Replay instances** are all replaying data to **Domain 0** by default.

## Usage

Please, refer to the original README file for general instructions.

## Notes

- Usage of Environment Variables to configure Replay sessions:
    This is done as you need XML for the configuration.
    Can be seen in code in `ReplayConfig` struct and
      `replay_service_property_new` function

- Usage of `JOURNAL_MODE = WAL` for the Database:
    This is needed to ensure proper performance from SQLite.
      See [SQLite docs](https://www.sqlite.org/wal.html).