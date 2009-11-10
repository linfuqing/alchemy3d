package
{
	import br.com.stimuli.loading.BulkLoader;
	import br.com.stimuli.loading.BulkProgressEvent;
	
	import cn.alchemy3d.container.Entity;
	import cn.alchemy3d.external.MD2;
	import cn.alchemy3d.external.Primitives;
	import cn.alchemy3d.geom.Mesh3D;
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.render.Material;
	import cn.alchemy3d.render.RenderMode;
	import cn.alchemy3d.render.Texture;
	import cn.alchemy3d.tools.Basic;
	import cn.alchemy3d.tools.FPS;
	
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.geom.ColorTransform;
	import flash.net.URLLoader;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.ui.Keyboard;

	[SWF(width="640",height="480",backgroundColor="#000000",frameRate="60")]
	public class TestMD2 extends Basic
	{
		protected var bl:BulkLoader;
		
		private var loader:URLLoader;
		
		//protected var p:Plane;
		
		protected var md2:MD2;
		protected var p:Primitives;
		protected var p2:Primitives;
		protected var p3:Primitives;
		
		protected var t0:Texture;
		protected var t1:Texture;
		
		protected var lightObj:Entity;
		protected var lightObj2:Entity;
		protected var lightObj3:Entity;
		
		protected var light:Light3D;
		protected var light2:Light3D;
		protected var light3:Light3D;
		
		protected var center:Entity;
		
		public function TestMD2()
		{
			super(640, 480, 90, 10, 5000);
			
			bl = new BulkLoader("main-site");
			bl.addEventListener(BulkProgressEvent.COMPLETE, init);
			bl.add("asset/wood01.jpg", {id:"2"});
			bl.start();
			
			//init();
		}
		
		private function showInfo():void
		{
			var fps:FPS = new FPS(scene);
			addChild(fps);
			
			var tformat:TextFormat = new TextFormat("宋体", 16, 0xffffff, true);
			var tf:TextField = new TextField();
			tf.defaultTextFormat = tformat;
			tf.autoSize = "left";
			tf.text = "MD2加载器Demo";
			tf.x = 280;
			tf.y = 20;
			addChild(tf);
		}
		
		protected function init(e:Event = null):void
		{
			bl.removeEventListener(BulkProgressEvent.COMPLETE, init);
			t0 = new Texture(bl.getBitmapData("2"));
			t0.perspectiveDist = 3000;
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(.2,.2,.2, 1);
			m.diffuse = new ColorTransform(.4, .4, .4, .5);
			m.specular = new ColorTransform(0, 0, 0, 1);
			
			var m2:Material = new Material();
			m2.ambient = new ColorTransform(.2, .2, .2, 1);
			m2.diffuse = new ColorTransform(0, 1, 0, .5);
			m2.specular = new ColorTransform(0, 0, 0, 1);
			
			p2 = new Primitives(m, null, RenderMode.RENDER_TRIANGLE_FSINVZB_ALPHA_32);
			p2.toPlane(300, 300, 1, 1);
//			p.mesh.fogEnable = true;
			p2.z = 200;
//			p2.x = 200;
//			p2.y = -100;
//			p2.rotationY = 45;
//			this.viewport.scene.addChild(p2);
			
			p3 = new Primitives(m, null, RenderMode.RENDER_TRIANGLE_FSINVZB_ALPHA_32);
			p3.toPlane(300, 300, 1, 1);
			p3.z = 400;
			p3.x = 150;
//			this.viewport.scene.addChild(p3);
			
			p = new Primitives(m2, t0, RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32);
			p.toPlane(400, 400, 1, 1);
//			p.mesh.useMipmap = true;
//			p.mesh.mipDist = 500;
			p.mesh.texScaleX = 3;
			p.mesh.texScaleY = 3;
			p.mesh.texOffsetX = -30;
			p.mesh.texOffsetY = -30;
			p.mesh.addressMode = Mesh3D.ADDRESS_MODE_WRAP;
			p.z = 400;
			this.viewport.scene.addChild(p);

			startRendering();
			
			showInfo();
			
			stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
		}
		
		protected function onKeyDown(e:KeyboardEvent):void
		{
			if ( e.keyCode == Keyboard.UP )
				p.z += 10;
				
			if ( e.keyCode == Keyboard.DOWN )
				p.z -= 10;
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
//			p.rotationZ ++;
//			p.y++
			super.onRenderTick(e);
			
//			md2.z += 5;
			
			//center.z ++;
//			center.rotationY ++;
			
//			camera.target = center.worldPosition;
//			var mx:Number = viewport.mouseX / 500;
//			var my:Number = - viewport.mouseY / 500;
//			
//			camera.hover(mx, my, 10);
		}
	}
}