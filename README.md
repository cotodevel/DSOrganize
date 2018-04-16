DSorganize for ToolchainGenericDS. Based from version: 3.1129, since it seems to be the latest stable version.

Currently it compiles. But it's in heavy development so expect bugs. The SSL driver apparently is cyassl 1.4.0 so I added it and made it compatible with TGDS environment.
SSL could be used in webbrowser, IRC and others. That will be checked once DSOrganize is stable enough, which is not now.


DSOrganize dependencies:	

- libdt
- libfb
- libini
- libm_apm
- libpicture
- libvcard	
- libfatdragon:
	Problem: It's excluded, since it's a modified libfat + a cache system that seems to cause issues. So I removed the cache part.
	Solution: So I extended https://github.com/cotodevel/gccnewlibnano_to_fsfat (TGDS neat Filesystem driver) features to replace libfat features. Currently working.
		
- Proper TGDS template: arm7/arm9
- DSorganize 3.1129
	
Bonus:
I added the missing libraries DSorganize seemed to require, but luckily I was able to find them scattered around the internet.

Notes:
DSorganize is GPLv3, but since I use a GPLv2+ license for all Newlib NDS/TGDS code, they are compatible.

Coto
