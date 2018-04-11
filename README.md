DSorganize for ToolchainGenericDS. Based from version: 3.1129

Currently it does nothing, it does not even compile. But I'm getting there.
But since I got openssl-0.9.1c.tar.gz to compile, I think I am far enough to submit the source.


what's compiling:
DSOrganize dependencies that is:
	-libdt
	-libfb
	-libini
	-libm_apm
	-libpicture
	-libvcard
	
	-libfatdragon:
		Problem: Is excluded, since it's a modified libfat + a cache system that seems to cause issues. 
		Solution: So I extended gccnewlibnano_to_fsfat features to support libfat features, and they work nicely now. (so it counts as dependency)

	-proper TGDS template: arm7/arm9
	
what's not compiling yet:
	-DSorganize 3.1129
	
Bonus:
I added the missing libraries DSorganize seemed to require, but luckily I was able to find them scattered around the internet.

Notes:
DSorganize is GPLv3, but since I use a GPLv2+ license for all Newlib NDS/TGDS code, they are compatible.

Coto
