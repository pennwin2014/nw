
Ext.define('MyDesktop.xtyhWindow', {
    extend: 'Ext.ux.desktop.Module',

   

    id:'xtyh-win',

    init : function(){
        this.launcher = {
            text: 'ϵͳ�û�',
            iconCls:'icon-grid'
        };
    },

    createWindow : function(){
        var desktop = this.app.getDesktop();
        var win = desktop.getWindow('xtyh-win');
        var htmlvar='<iframe src="/pronline/Msg?FunName@ncsWebReplace&plate@windows/main_frm.htm&title@ϵͳ�û�" style="width:1350px;height:950px;display:block;" FrameBorder=0 scrolling="yes">';
        if(!win){
            win = desktop.createWindow({
                id: 'xtyh-win',
                title:'ϵͳ�û�',
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

