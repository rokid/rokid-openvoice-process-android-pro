# rokid-openvoice-sample-android
##Rokid语音服务的Android示例代码文档
此部分是Rokid语音服务（Rokid Voice Services）的Android示例代码，允许开发者快速搭建一套Rokid语音服务以体验Rokid提供的语音交互能力。

## 内容
1.请编译speech的[配置](https://developer-forum.rokid.com/t/rokid/101)文件到/system/etc/speech_sdk.json

2.请添加如下环境变量到system/core/rootdir/init.environ.rc.in

	export GRPC_DEFAULT_SSL_ROOTS_FILE_PATH /system/etc/roots.pem

3.请修改如下位置：

	rokid-openvoice-sample-android/Android.mk
	OPENVOICE_DIR_DEP = robot/openvoice			修改为自己的SDK路径
