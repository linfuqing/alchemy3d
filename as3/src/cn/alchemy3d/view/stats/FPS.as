package cn.alchemy3d.view.stats
{
    import cn.alchemy3d.scene.Scene3D;
    
    import flash.display.Bitmap;
    import flash.display.BitmapData;
    import flash.display.Sprite;
    import flash.events.Event;
    import flash.geom.Point;
    import flash.geom.Rectangle;
    import flash.system.System;
    import flash.text.TextField;
    import flash.text.TextFieldAutoSize;
    import flash.text.TextFormat;
    import flash.utils.getTimer;

    public class FPS extends Sprite
    {
        private var currentY:int;
        private var diagramTimer:int;
        private var tfTimer:int;
        private var mem:TextField;
        private var faces:TextField;
        private var diagram:BitmapData;
        private var skins:int = -1;
        private var fps:TextField;
        private var tfDelay:int = 0;
        private var skinsChanged:int = 0;
        private var scene:Scene3D;
        
        static private var instance:FPS;
        static private const maxMemory:uint = uint.MAX_VALUE;
        static private const diagramWidth:uint = 60;
        static private const tfDelayMax:int = 10;
        static private const diagramHeight:uint = 40;

        public function FPS(scene:Scene3D = null)
        {
        	this.scene = scene;
        	
            this.addEventListener(Event.ADDED_TO_STAGE,run);
        }

		private function run(ev:Event):void
		{
			var bmp:Bitmap;
            fps = new TextField();
            mem = new TextField();
            if (instance == null)
            {
                currentY = scene ? 30 : 20;
                
                mouseEnabled = false;
                mouseChildren = false;
                
                var tf:TextFormat = new TextFormat("Tahoma", 10, 13421772);
                
                fps.defaultTextFormat = tf;
                fps.autoSize = TextFieldAutoSize.LEFT;
                fps.text = "FPS: " + Number(stage.frameRate).toFixed(2);
                fps.selectable = false;
                fps.x = -diagramWidth - 2;
                addChild(fps);
                
                mem.defaultTextFormat = tf;
                mem.autoSize = TextFieldAutoSize.LEFT;
                mem.text = "MEM: " + bytesToString(System.totalMemory);
                mem.selectable = false;
                mem.x = -diagramWidth - 2;
                mem.y = 10;
                addChild(mem);
                
                if (scene)
                {
	          		faces = new TextField();
	                faces.defaultTextFormat = tf;
	                faces.autoSize = TextFieldAutoSize.LEFT;
	                faces.text = "FACES: " + scene.facesNum;
	                faces.selectable = false;
	                faces.x = -diagramWidth - 2;
	                faces.y = 20;
	                addChild(faces);
                }
                
                diagram = new BitmapData(diagramWidth, diagramHeight, true, 0x20ffffff);
                bmp = new Bitmap(diagram);
                bmp.y = currentY + 4;
                bmp.x = -diagramWidth;
                addChildAt(bmp, 0);
                addEventListener(Event.ENTER_FRAME, onEnterFrame);
                this.stage.addEventListener(Event.RESIZE, onResize);
                onResize();
                diagramTimer = getTimer();
                tfTimer = getTimer();
            }
		}
        private function bytesToString(value:uint) : String
        {
            var str:String;
            if (value < 1024)
            {
                str = String(value) + "b";
            }
            else if (value < 10240)
            {
                str = Number(value / 1024).toFixed(2) + "kb";
            }
            else if (value < 102400)
            {
                str = Number(value / 1024).toFixed(1) + "kb";
            }
            else if (value < 1048576)
            {
                str = (value / 1024 >> 0) + "kb";
            }
            else if (value < 10485760)
            {
                str = Number(value / 1048576).toFixed(2) + "mb";
            }
            else if (value < 104857600)
            {
                str = Number(value / 1048576).toFixed(1) + "mb";
            }
            else
            {
                str = (value / 1048576 >> 0) + "mb";
            }// end else if
            return str;
        }

        private function onEnterFrame(e:Event) : void
        {
            tfDelay++;
            if (tfDelay >= tfDelayMax)
            {
                tfDelay = 0;
                fps.text = "FPS: " + Number(1000 * tfDelayMax / (getTimer() - tfTimer)).toFixed(2);
                tfTimer = getTimer();
            }
            var t:Number = 1000 / (getTimer() - diagramTimer);
            var f:Number = t > stage.frameRate ? (1) : (t / stage.frameRate);
            diagramTimer = getTimer();
            diagram.scroll(1, 0);
            diagram.fillRect(new Rectangle(0, 0, 1, diagram.height), 0x20ffffff);
            diagram.setPixel32(0, diagramHeight * (1 - f), 0xffcccccc);
            
            mem.text = "MEM: " + bytesToString(System.totalMemory);
            if (scene) faces.text = "FACES: " + scene.facesNum;
            
            var s:Number = skins == 0 ? (0) : (skinsChanged / skins);
            diagram.setPixel32(0, diagramHeight * (1 - s), 0xffff6600);
            var m:Number = System.totalMemory / maxMemory;
            diagram.setPixel32(0, diagramHeight * (1 - m), 0xffcccc00);
        }

        private function onResize(e:Event = null):void
        {
            var p:Point = parent.globalToLocal(new Point(this.width, 0));
            x = p.x;
            y = p.y;
        }
    }
}
