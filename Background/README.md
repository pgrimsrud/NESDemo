This is using Nes Screen Tool which:
  1. Can only handle one nametable at a time
  2. Uses the Session to save info like chr and palette (currently hard coded)
  3. Compresses the level. Saves a lot of space IF there are similar tiles in a row (else might want a different alg)

To Edit, open the .nam file in the tool. You need to save both the nam (so you can reopen it later) and the .h file (save as RLE Packed as C header) for actual use. Be sure to save nametable AND attributes.

TODO script to output collision based on the map and storing it outside of code as well
