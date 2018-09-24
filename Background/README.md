This is using Nes Screen Tool which:
  1. Can only handle one nametable at a time
  2. Uses the Session to save info like chr and palette (currently hard coded)
  3. Compresses the level. Saves a lot of space IF there are similar tiles in a row (else might want a different alg)

To Edit, open the .nam file in the tool. You need to save both the nam (so you can reopen it later) and the .h file (save as RLE Packed as C header) for actual use. Be sure to save nametable AND attributes.

To generate collision map, save the file as a NON RLE .h file in the format "Level<#><side>Collision.h" where '<#>' is the level number and '<side>' is Top or Bottom. Run the scrypt with "python parseCollision.py <#>" and it'll parse the top and bottom and spit out the collision map

