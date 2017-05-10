# rokid-openvoice-sample-android
##Rokid语音服务的Android示例代码文档此部分是Rokid语音服务（Rokid Voice Services）的Android示例代码，允许开发者快速搭建一套Rokid语音服务以体验Rokid提供的语音交互能力。使用指南请点[这里](https://developer-forum.rokid.com/t/rokid-open-voice-sdk/97/10)

## 内容
1.请编译speech的[配置](https://developer-forum.rokid.com/t/rokid/101)文件到/system/etc/openvoice_profile.json

	{
		'server_address': 'apigw.open.rokid.com:443',
		'ssl_roots_pem': '/system/etc/roots.pem',
		'key': 'your_key',
		'device_type_id': 'your_device_type_id',
		'device_id': 'your_device_id',
		'api_version': '1',
		'secret': 'your_secret'
		'codec': 'opu'
	}

2.请到system/core/init/init.cpp修改selinux的工作模式
	
	static selinux_enforcing_status selinux_status_from_cmdline() {
		selinux_enforcing_status status = SELINUX_ENFORCING;
		修改为：
		selinux_enforcing_status status = SELINUX_PERMISSIVE;
		. . .	. . . 
	}

3.请修改如下内容rokid-openvoice-sample-android/Android.mk

	OPENVOICE_DIR_DEP = robot/openvoice	//修改为自己的SDK路径

4.请修改如下内容rokid-openvoice-sample-android/device/xxxx/p230/sample.mk

	mic_array.p230 //修改为自己的平台名称(p230)

5.请添加如下内容到device/xxxx/p230/p230.mk
	
	include rokid-openvoice-sample-android/device/xxxx/p230/sample.mk

6.请在build/core/definitions.mk最后一行添加如下内容

	include rokid-openvoice-sample-android/build/core/definition_exts.mk

7.请在联网下调试
