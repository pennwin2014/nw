
Ext.define('MyDesktop.ZxlWindow', {
    extend: 'Ext.ux.desktop.Module',

   

    id:'zxl-win',

    init : function(){
        this.launcher = {
            text: '������ͳ��',
            iconCls:'icon-grid'
        };
    },

    createWindow : function(){
        var desktop = this.app.getDesktop();
        var win = desktop.getWindow('zxl-win');
                      var htmlvar='<iframe src="/pronline/Msg?FunName@ncsWebReplace&plate@windows/main_frm.htm&title@������ͳ��" style="width:1350px;height:950px;display:block;" FrameBorder=0 scrolling="yes">';
        if(!win){
            win = desktop.createWindow({
                id: 'zxl-win',
                title:'��λ������ͳ��',
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

