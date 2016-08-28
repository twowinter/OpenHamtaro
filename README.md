# OpenHamtaro

## 项目背景
大家好，我是多多的爸爸，多多是在李志唱了“多多你不要哭”之后来到这个世界上的小女孩。

前段时间给多多买了个语音仓鼠玩具，主要功能就是你说一句，它重复一句，里头有个电机，讲话的时候电机跟着转动，相当于自带抽风效果。多多特别喜欢，整天抓在手里又亲又咬，仓鼠一抽风，她也乐地抽风。后来仓鼠坏掉了，怎么叫都叫不醒，多多以为是开关没打开，我跟她解释说仓鼠坏掉了，她听不懂，一直拉着我的手往底座去拨开关，让人怜惜。

机智云Cherry从CSDN联系到我说要赠送一个GoKit3的时候，我很开心，很早就知道它支持语音识别，我脑海中蹦出的想法就是利用Gokit3给闺女的仓鼠玩具做个大换血。记得5月的时候，周杰伦随手就给女儿写了首歌《前世情人》，让我嫉妒不已，老婆鼓励我说“会写代码的爸爸同样牛逼”。这次我也终于能够得偿所愿，给多多写段代码了。

这就是这个开源项目的由来。

![](http://7xkqvo.com1.z0.glb.clouddn.com/open_hamtaro_kiss_ok.jpg)  

## 项目介绍
OpenHamtaro是一个以仓鼠玩偶为原型的语音机器人项目，具备语音交互能力，可与不同品牌的ZigBee智能家居产品互联互通。项目希望通过开源方式，集合大家的力量，一起打造一个开放自由的智能家居网络，让大家可以挑选喜欢的设备进入个人生活。

OpenHamtaro有如下特点:  

- 1.语音交互。  
Apple有Siri，Google有Goolge Now，你说“OK glass”可以与谷歌眼镜交谈，你说“Alexa”可以让亚马逊Echo播放音乐。 在OpenHamtaro，你念叨下“仓鼠管家”，就可以操作智能家居设备了。

- 2.ZigBee互联互通。  
在智能家居的应用领域，ZigBee在互联互通、网络接入数量等方面都比WiFi要更有优势。虽然ZigBee联盟已经公布了有1072款的认证产品，但似乎离我们还比较远。
希望通过这个OpenHamtaro项目，逐个接入各个品牌的ZigBeeHomeAutomation智能家居单品，比如飞利浦的HUE灯，南京物联的插座，施耐德的开关等等。甚至给小米的ZigBee智能家居套装重写满足ZHA协议的固件，用于接入标准ZHA网络。（如果绿米的工程师看到这，希望官方能发布一个标准ZHA固件，实现与其他ZHA产品的互联互通。）

- 3.全球领先的GreenPower协议实现。  
这两年免电池开关很受欢迎，Enocean联盟、以及国内的领普等专做无源免电池设备的厂商都大火。其实早在2012年ZigBee协议中就加入对无源免电池设备的兼容，但是技术上一直攻克不下来。最近从上游供应商那传来喜讯，有了工程样机，期待能在这个项目中实现这个技术的接入。

总的来说，OpenHamtaro要做一个开放包容且有意思的智能家居网关，这也是这个“产品”的差异化特点。

## 一期项目成果展示  

### 原型照片  

GoKit3+仓鼠玩偶  
![](http://7xkqvo.com1.z0.glb.clouddn.com/open_hamtaro_prototype_ok.jpg)    

飞利浦HUE闪亮登场，感谢领导大力支持。  
![](http://7xkqvo.com1.z0.glb.clouddn.com/open_hamtaro_hue.jpg)   


### 功能演示视频  

严肃演示版视频(锤子T1闪亮出境)：    
[http://v.qq.com/x/page/z0324xe9wtm.html](http://v.qq.com/x/page/z0324xe9wtm.html)

语音调戏版视频(仓鼠管家抽风特写)：  
[http://v.qq.com/x/page/p0324kjqakq.html](http://v.qq.com/x/page/p0324kjqakq.html)

## 总体设计方案  

### 系统拓扑  
![](http://7xkqvo.com1.z0.glb.clouddn.com/open_hamtaro_topo.png)  


### BOM  
![](http://7xkqvo.com1.z0.glb.clouddn.com/open_hamtaro_bom.png)  


### 项目计划  

- 一期计划(20160728~20160831):  
以GoKit3为硬件原型机，主要实现软件功能验证。  
![](http://7xkqvo.com1.z0.glb.clouddn.com/open_hamtaro_plan.png)  

- 二期计划(20160901~20161131):    
硬件方面可以开工，着手制作样机。软件方面，接入更多的HA设备，如无源开关等。  



