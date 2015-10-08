
Ext.define('MyDesktop.ZxdwWindow', {
    extend: 'Ext.ux.desktop.Module',

   

    id:'zxdw-win',

    init : function(){
        this.launcher = {
            text: '在线单位',
            iconCls:'icon-grid'
        };
    },

    createWindow : function(){
        var desktop = this.app.getDesktop();
        var win = desktop.getWindow('gdrz-win');
         var htmlvar='<iframe src="/v4/windows/main_zxdw.htm" style="width:1150px;height:950px;display:block;" FrameBorder=0 scrolling="yes">';
        if(!win){
            win = desktop.createWindow({
                id: 'zxdw-win',
                title:'在线单位',
                width:1024,
                height:700,
                autoScroll: true,
                iconCls: 'icon-grid',
                animCollapse:false,
                constrainHeader:true,
    //            layout: 'fit',
                 html:htmlvar

            });
        }
        return win;
    }
});

