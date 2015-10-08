{root: {
			text: "全部权限",
			checked: false,
			expanded: true,
			children: [
				{ id: "实时监控",text: "实时监控", checked: false, leaf: false, expanded: true, children: [
					{ id: 'sjcomputer',text: "在线计算机", checked: false, leaf: false, expanded: true, children: [
						{ id: 1110,text: "图标方式", checked: false, leaf: true},
						{ id: 1120,text: "列表方式", checked: false, leaf: true}
					]},
					{ id: 'sspm',text: "实时屏幕", checked: false, leaf: true},
					{ id: 'immon',text: "实时聊天", checked: false, leaf: true},
					{ id: 'ssjc',text: "实时进程", checked: false, leaf: true}
				]},
				{ id: "控制策略",text: "控制策略", checked: false, leaf: false, expanded: true, children: [
					{ id: "tree_procy,center_procy",text: "策略控制", checked: false, leaf: false, expanded: true, children: [
						{ id: 'file',text: "文件控制", checked: false, leaf: false, expanded: true, children: [
							{ id: 2110,text: "增加", checked: false, leaf: true},
							{ id: 2120,text: "修改", checked: false, leaf: true},
							{ id: 2130,text: "删除", checked: false, leaf: true}
						]},
						{ id: 'dev',text: "外设控制", checked: false, leaf: false, expanded: true, children: [
							{ id: 2210,text: "增加", checked: false, leaf: true},
							{ id: 2220,text: "修改", checked: false, leaf: true},
							{ id: 2230,text: "删除", checked: false, leaf: true}
						]},
						{ id: 'link',text: "非法外联控制", checked: false, leaf: false, expanded: true, children: [
							{ id: 2310,text: "增加", checked: false, leaf: true},
							{ id: 2320,text: "修改", checked: false, leaf: true},
							{ id: 2330,text: "删除", checked: false, leaf: true}
						]},
						{ id: 'pgtype',text: "应用程序控制", checked: false, leaf: false, expanded: true, children: [
							{ id: 2410,text: "增加", checked: false, leaf: true},
							{ id: 2420,text: "修改", checked: false, leaf: true},
							{ id: 2430,text: "删除", checked: false, leaf: true}
						]},
						{ id: 'pname',text: "进程控制", checked: false, leaf: false, expanded: true, children: [
							{ id: 2510,text: "增加", checked: false, leaf: true},
							{ id: 2520,text: "修改", checked: false, leaf: true},
							{ id: 2530,text: "删除", checked: false, leaf: true}
						]},
						{ id: 'net',text: "网络控制", checked: false, leaf: false, expanded: true, children: [
							{ id: 2610,text: "增加", checked: false, leaf: true},
							{ id: 2620,text: "修改", checked: false, leaf: true},
							{ id: 2630,text: "删除", checked: false, leaf: true}
						]},
						{ id: 'url',text: "网址控制", checked: false, leaf: false, expanded: true, children: [
							{ id: 2710,text: "增加", checked: false, leaf: true},
							{ id: 2720,text: "修改", checked: false, leaf: true},
							{ id: 2730,text: "删除", checked: false, leaf: true}
						]},
						{ id: 'screen',text: "日志传送控制", checked: false, leaf: false, expanded: true, children: [
							{ id: 2810,text: "增加", checked: false, leaf: true},
							{ id: 2820,text: "修改", checked: false, leaf: true},
							{ id: 2830,text: "删除", checked: false, leaf: true}
						]},
						{ id: 'preid',text: "使用预定义策略", checked: false, leaf: false, expanded: true, children: [
							{ id: 2910,text: "增加", checked: false, leaf: true},
							{ id: 2920,text: "修改", checked: false, leaf: true},
							{ id: 2930,text: "删除", checked: false, leaf: true}
						]}
					]},
					{ id: "xfcl",text: "策略下发", checked: false, leaf: false, expanded: true, children: []},
				]},
				{ id: '日志查看',text: "日志查看", checked: false, leaf: false, expanded: true, children: [
					{ id: 'imcontent',text: "聊天内容", checked: false, leaf: false, expanded: true, children: [
						{ id: 3110,text: "导出", checked: false, leaf: true}
					]},
					{ id: 'screenlog',text: "屏幕日志", checked: false, leaf: true},
					{ id: 'urllog',text: "网站访问日志", checked: false, leaf: false, expanded: true, children: [
						{ id: 3310,text: "导出", checked: false, leaf: true}
					]},
					{ id: 'proclog',text: "进程使用日志", checked: false, leaf: false, expanded: true, children: [
						{ id: 3410,text: "导出", checked: false, leaf: true}
					]},
					{ id: 'waishelog',text: "外设使用日志", checked: false, leaf: false, expanded: true, children: [
						{ id: 3510,text: "导出", checked: false, leaf: true}
					]},
					{ id: 'filelog',text: "文件操作日志", checked: false, leaf: false, expanded: true, children: [
						{ id: 3610,text: "导出", checked: false, leaf: true}
					]},
					{ id: 'warnlog',text: "告警日志", checked: false, leaf: false, expanded: true, children: [
						{ id: 3710,text: "导出", checked: false, leaf: true}
					]},
				]},
				{ id: '计算机信息',text: "计算机信息", checked: false, leaf: false, expanded: true, children: [
					{ id: 'compbaseinfo',text: "基本信息", checked: false, leaf: true},
					{ id: 'comphardinfo',text: "硬件信息", checked: false, leaf: true},
					{ id: 'compsoftinfo',text: "软件信息", checked: false, leaf: true},
					{ id: 'compservice',text: "服务信息", checked: false, leaf: false, expanded: true, children: [
						{ id: 4410,text: "操作", checked: false, leaf: true}
					]},
					{ id: 'compssjc',text: "实时进程", checked: false, leaf: false, expanded: true, children: [
						{ id: 4510,text: "操作", checked: false, leaf: true}
					]},
					{ id: 'compdiskinfo',text: "磁盘信息", checked: false, leaf: true},
					{ id: 'compnetlinkinfo',text: "网络连接信息", checked: false, leaf: true}
				]},
				{ id: '基础信息',text: "基础信息", checked: false, leaf: false, expanded: true, children: [
					{ id: 'client',text: "客户端信息", checked: false, leaf: false, expanded: true, children: [
						{ id: 5110,text: "导出硬件列表", checked: false, leaf: true},
						{ id: 5120,text: "删除", checked: false, leaf: true},
						{ id: 5130,text: "关机", checked: false, leaf: true},
						{ id: 5140,text: "重启", checked: false, leaf: true},
						{ id: 5150,text: "卸载客户端", checked: false,  leaf: true},
						{ id: 5160,text: "移动部门", checked: false,  leaf: true}
					]},
					{ id: 'dept',text: "部门管理", checked: false,  leaf: false, expanded: true, children: [
						{ id: 5210,text: "添加部门", checked: false,  leaf: true},
						{ id: 5220,text: "删除部门", checked: false,  leaf: true}
					]},
					{ id: 'lay_base',text: "基础信息定义", checked: false,  leaf: false, expanded: true, children: [
						{ id: 'soft',text: "软件类型定义", checked: false,  leaf: false, expanded: true, children: [
							{ id: 5311,text: "添加软件类型", checked: false,  leaf: true},
							{ id: 5312,text: "删除软件类型", checked: false,  leaf: true},
							{ id: 5313,text: "添加软件", checked: false,  leaf: true},
							{ id: 5314,text: "删除软件", checked: false,  leaf: true}
						]},
						{ id: 'timeobj',text: "时间对象定义", checked: false,  leaf: false, expanded: true, children: [
							{ id: 5321,text: "添加时间类型", checked: false,  leaf: true},
							{ id: 5322,text: "删除时间类型", checked: false, leaf: true},
							{ id: 5323,text: "添加时间", checked: false,  leaf: true},
							{ id: 5324,text: "删除时间", checked: false,  leaf: true}
						]},
						{ id: 'worktime',text: "工作时间定义", checked: false,  leaf: false, expanded: true, children: [
							{ id: 5331,text: "添加", checked: false,  leaf: true},
							{ id: 5332,text: "删除", checked: false,  leaf: true}
						]},
						{ id: 'usb',text: "USB类型定义", checked: false,  leaf: false, expanded: true, children: [
							{ id: 5341,text: "添加USB类型", checked: false,  leaf: true},
							{ id: 5342,text: "删除USB类型", checked: false,  leaf: true},
							{ id: 5343,text: "添加USB", checked: false,  leaf: true},
							{ id: 5344,text: "删除USB", checked: false,  leaf: true}
						]}
					]}
				]},
				/*{ id: '系统管理',text: "系统管理", checked: false,  leaf: false, expanded: true, children: [
					{ id: 'rightrole',text: "权限角色", checked: false,  leaf: false, expanded: true, children: [
						{ id: 6110,text: "增加", checked: false,  leaf: true},
						{ id: 6120,text: "修改", checked: false,  leaf: true},
						{ id: 6130,text: "删除", checked: false,  leaf: true},
						{ id: 6140,text: "授权给对象", checked: false,  leaf: true}
					]},
					{ id: 'adminuser',text: "系统用户", checked: false,  leaf: false, expanded: true, children: [
						{ id: 6210,text: "增加", checked: false,  leaf: true},
						{ id: 6220,text: "修改", checked: false,  leaf: true},
						{ id: 6230,text: "删除", checked: false,  leaf: true}
					]},
					{ id: 'operlog',text: "系统操作日志", checked: false,  leaf: true}
				]},
				{ id: '客户端升级',text: "客户端升级", checked: false,  leaf: false, expanded: true, children: [
					{ id: 'updatesoft',text: "软件上传", checked: false,  leaf: false, expanded: true, children: [
						{ id: 7110,text: "软件上传", checked: false,  leaf: true},
						{ id: 7120,text: "删除", checked: false,  leaf: true}
					]},
					{ id: 'softdown',text: "软件下发", checked: false,  leaf: false, expanded: true, children: [
						{ id: 7210,text: "增加", checked: false,  leaf: true},
						{ id: 7220,text: "删除", checked: false,  leaf: true},
						{ id: 7230,text: "应用", checked: false,  leaf: true}
					]},
					{ id: 'downlog',text: "日志升级", checked: false,  leaf: true}
				]},
				{ id: '系统设置',text: "系统设置", checked: false,  leaf: false, expanded: true, children: [
					{ id: 'ipset',text: "IP地址设置", checked: false,  leaf: true},
					{ id: 'dataclean',text: "数据存储设置", checked: false,  leaf: true},
					{ id: 'databack',text: "备份设置", checked: false,  leaf: true},
					{ id: 'dispfield',text: "电脑树显示字段", checked: false,  leaf: true},
					{ id: 'shutdown',text: "关机和重启", checked: false,  leaf: true}
				]}, */
				{ id: '安装包下载',text: "安装包下载", checked: false,  leaf: true}
			]
		}
	}