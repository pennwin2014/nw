
Ext.define('MyDesktop.ZxdwWindow', {
    extend: 'Ext.ux.desktop.Module',

   

    id:'zxdw-win',

    init : function(){
        this.launcher = {
            text: '���ߵ�λ',
            iconCls:'icon-grid'
        };
    },

    createWindow : function(){
        var desktop = this.app.getDesktop();
        var win = desktop.getWindow('gdrz-win');
                var htmlvar='<iframe src="/pronline/Msg?FunName@ncsWebReplace&plate@windows/main_frm.htm&title@���ߵ�λ" style="width:1350px;height:950px;display:block;" FrameBorder=0 scrolling="yes">';
        if(!win){
            win = desktop.createWindow({
                id: 'zxdw-win',
                title:'���ߵ�λ',
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

