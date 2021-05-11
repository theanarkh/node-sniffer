#include <stdio.h>
#include <errno.h>  
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>  
#include <linux/in.h>
#include <linux/if_ether.h>
#include <stdlib.h>
#include <node_api.h>

#define DATA_LEN 500

static napi_value start(napi_env env, napi_callback_info info) {
	int sockfd;
	int bytes;
	char data[DATA_LEN];
	unsigned char *ipHeader;
	unsigned char *macHeader;
	unsigned char *transportHeader;
	sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP));
	if (sockfd < 0) {
		printf("创建socket错误");
		exit(1);
	}

	while (1) {
		bytes = recvfrom(sockfd,data,DATA_LEN,0,NULL,NULL);
		printf("读到字节数：%d\n",bytes);
		macHeader = data;
		printf("MAC报文----------\n");
		printf("源Mac地址: %02x:%02x:%02x:%02x:%02x:%02x\n",
		       macHeader[0],macHeader[1],macHeader[2],
		       macHeader[3],macHeader[4],macHeader[5]);
		printf("目的Mac地址: %02x:%02x:%02x:%02x:%02x:%02x\n",
		       macHeader[6],macHeader[7],macHeader[8],
		       macHeader[9],macHeader[10],macHeader[11]);
		printf("上层协议: %04x\n",
		       (macHeader[12] << 8) + macHeader[13]);
		// 跳过Mac头
		ipHeader = data + 6 + 6 + 2;
		printf("IP报文--------\n");
		printf("ip协议版本：%d\n",
		         (ipHeader[0] & 0xF0) >> 4); 
		int ipHeaderLen = (ipHeader[0] & 0x0F) << 2;
		printf("首部长度：%d\n",
		     ipHeaderLen);
		printf("区分服务：%d\n",
		     ipHeader[1]);     
		printf("总长度：%d\n",
		     (ipHeader[2]<<8)+ipHeader[3]); 
		printf("标识：%d\n",
		     (ipHeader[4]<<8)+ipHeader[5]);
		printf("标志：%d\n",
		     (ipHeader[6] & 0xE0) >> 5);     
		printf("片偏移：%d\n",
		     (ipHeader[6] & 0x11) + ipHeader[7]);  
		printf("TTL：%d\n",
		     ipHeader[8]);
		printf("上层协议：%d\n",
		     ipHeader[9]);     
		printf("首部校验和：%x%x\n",
		     ipHeader[10]+ipHeader[11]);                          
		printf("源ip：%d.%d.%d.%d\n",
		     ipHeader[12],ipHeader[13],
		     ipHeader[14],ipHeader[15]);
		printf("目的ip：%d.%d.%d.%d\n",
		     ipHeader[16],ipHeader[17],
		     ipHeader[18],ipHeader[19]);

		transportHeader = ipHeader + ipHeaderLen;
		printf("传输层报文-----------\n");
		printf("源端口：%d\n",
		     (transportHeader[0]<<8)+transportHeader[1]);
		printf("目的端口：%d\n",
		     (transportHeader[2]<<8)+transportHeader[3]);
		printf("序列号：%ud%ud%ud%ud\n",
		     transportHeader[4],transportHeader[5],transportHeader[6],transportHeader[7]);
		printf("确认号：%ud\n",
		     (transportHeader[8]<<24)+(transportHeader[9]<<16)+(transportHeader[10]<<8)+(transportHeader[11]));
		printf("传输层首部长度：%d\n",
		    ((transportHeader[12] & 0xF0) >> 4) * 4);
		printf("FIN：%d\n",
		    transportHeader[13] & 0x01);
		printf("SYN：%d\n",
		    (transportHeader[13] & 0x02) >> 1);
		printf("RST：%d\n",
		    (transportHeader[13] & 0x04) >> 2);
		printf("PSH：%d\n",
		    (transportHeader[13] & 0x08) >> 3);
		printf("ACK：%d\n",
		    (transportHeader[13] & 0x016) >> 4);
		printf("URG：%d\n",
		    (transportHeader[13] & 0x32) >> 5);
		printf("窗口大小：%d\n",
		    (transportHeader[14] << 8) + transportHeader[15]);
  	}
}

napi_value Init(napi_env env, napi_value exports) {
	napi_value func;
	napi_create_function(env,
	                  NULL,
	                  NAPI_AUTO_LENGTH,
	                  start,
	                  NULL,
	                  &func);
	napi_set_named_property(env, exports, "start", func);
	return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)