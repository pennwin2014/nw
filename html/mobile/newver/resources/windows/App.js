
        var data=new Array();
        data=[      { name: '�鵵��־', iconCls: 'grid-shortcut', module: 'gdrz-win' },
                    { name: '��λ����', iconCls: 'accordion-shortcut', module: 'dwgl-win' },
                    { name: '������ͳ��', iconCls: 'cpu-shortcut', module: 'zxl-win' },
                    { name: '���ع���', iconCls: 'notepad-shortcut', module: 'bkgl-win' },
                    { name: '���ظ澯��ѯ', iconCls: 'grid-shortcut', module: 'bkgj-win' },
                    { name: '��λ�����', iconCls: 'grid-shortcut', module: 'dwz-win' },
                    { name: '���ߵ�λ', iconCls: 'grid-shortcut', module: 'zxdw-win' },
                    { name: 'ϵͳ�û�', iconCls: 'grid-shortcut', module: 'xtyh-win' },
                    { name: '���з���', iconCls: 'grid-shortcut', module: 'yxfx-win' },
                    { name: '���м��', iconCls: 'grid-shortcut', module: 'mgjk-win' },
                    { name: '���й���', iconCls: 'grid-shortcut', module: 'mggz-win' }
                ];
Ext.define('MyDesktop.App', {
    extend: 'Ext.ux.desktop.App',

    requires: [
        'Ext.window.MessageBox',
        'Ext.ux.desktop.ShortcutModel',
        'MyDesktop.DwglWindow',
        'MyDesktop.GdrzWindow',
        'MyDesktop.ZxlWindow',
        'MyDesktop.DwzWindow',
        'MyDesktop.BkglWindow',
        'MyDesktop.BkgjWindow',
        'MyDesktop.ZxdwWindow',
        'MyDesktop.xtyhWindow',        
        'MyDesktop.YxfxWindow',
        'MyDesktop.MgjkWindow',
        'MyDesktop.MggzWindow',
        'MyDesktop.Settings'
    ],

    init: function() {
        // custom logic before getXYZ methods get called...

        this.callParent();

        // now ready...
    },

    getModules : function(){
    	var startm=new Array();
    	  var rfun=right_fun;
			
				var url_sy;
				var right_id;
        var i,m;
        m=0;
        var datam=new Array();
           datam=[   
            new MyDesktop.DwglWindow(),
            new MyDesktop.GdrzWindow(),
            new MyDesktop.ZxlWindow(),
            new MyDesktop.ZxdwWindow(),
            new MyDesktop.BkgjWindow(),
            new MyDesktop.BkglWindow(),
            new MyDesktop.DwzWindow(),
            new MyDesktop.YxfxWindow(),
            new MyDesktop.MgjkWindow(),
            new MyDesktop.MggzWindow()          
        ];
        var ll=data.length;
    
          for(i=0;i<ll-1;i++){
        	 if(rfun.indexOf(data[i].name)!=-1){
        	 	startm[m]=datam[i];
        	 	m++
        	 }
        }
        return startm;
    },

    getDesktopConfig: function () {
        var me = this, ret = me.callParent();
        var data_aa=new Array();
        
        var rfun=right_fun;
			
				var url_sy;
				var right_id;
        var i,m;

        m=0;
        for(i=0;i<data.length;i++){
        	 if(rfun.indexOf(data[i].name)!=-1){
        	 	data_aa[m]=data[i];
        	 	m++
        	 }
        }
       
        	

        return Ext.apply(ret, {
            //cls: 'ux-desktop-black',

            contextMenuItems: [
                { text: 'Change Settings', handler: me.onSettings, scope: me }
            ],

            shortcuts: Ext.create('Ext.data.Store', {
                model: 'Ext.ux.desktop.ShortcutModel',
                data: data_aa
            }),

            wallpaper: '/newver/resources/windows/wallpapers/Blue-Sencha.jpg',
            wallpaperStretch: false
        });
    },

    // config for the start menu
    getStartConfig : function() {
        var me = this, ret = me.callParent();

        return Ext.apply(ret, {
            title: 'ϵͳ����Ա',
            iconCls: 'user',
            height: 300,
            toolConfig: {
                width: 100,
                items: [
                    {
                        text:'����',
                        iconCls:'settings',
                        handler: me.onSettings,
                        scope: me
                    },
                    '-',
                    {
                        text:'�˳�',
                        iconCls:'logout',
                        handler: me.onLogout,
                        scope: me
                    }
                ]
            }
        });
    },

    getTaskbarConfig: function () {
        var ret = this.callParent();
        var right_id;
        if(right_sy=='���м��'){
           right_id='mgjk-win';
          }
         else if(right_sy=='��λ����'){
         	right_id=='dwgl-win';
        }
        else if(right_sy=='���з���'){
         	right_id=='yxfx-win';
        }
	      else{
	      	  right_sy='��λ����';
	      		right_id=='dwgl-win';
	      }
        return Ext.apply(ret, {
            quickStart: [
               
                { name: right_sy, iconCls: 'accordion', module: right_id }
            ],
            trayItems: [
                { xtype: 'trayclock', flex: 1 }
            ]
        });
    },

    onLogout: function () {
        Ext.Msg.confirm('�˳�', '��ȷ��Ҫ�˳�ϵͳ��?');
    },

    onSettings: function () {
        var dlg = new MyDesktop.Settings({
            desktop: this.desktop
        });
        dlg.show();
    }
});
