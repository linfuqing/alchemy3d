package
{
	import br.com.stimuli.loading.BulkLoader;
	import br.com.stimuli.loading.BulkProgressEvent;
	
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.geom.Vertex3D;
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.lights.LightType;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.objects.primitives.Plane;
	import cn.alchemy3d.objects.primitives.Sphere;
	import cn.alchemy3d.render.RenderMode;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.texture.Texture;
	import cn.alchemy3d.view.Basic;
	import cn.alchemy3d.view.Viewport3D;
	import cn.alchemy3d.view.stats.FPS;
	
	import flash.display.StageAlign;
	import flash.display.StageQuality;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.geom.ColorTransform;
	import flash.text.TextField;
	import flash.text.TextFormat;
	
	import gs.TweenLite;

	[SWF(width="640",height="480",backgroundColor="#000000",frameRate="60")]
	public class Cloth extends Basic
	{
		protected var viewport:Viewport3D;
		protected var camera:Camera3D;
		protected var scene:Scene3D;
		
		protected var light:Light3D;
		protected var lightObj:Sphere;
		
		protected var p:Plane;
		
		protected var t1:Texture;
		
		protected var bl:BulkLoader;
		
		public function Cloth()
		{
			super();
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			stage.quality = StageQuality.BEST;
			stage.frameRate = 60;
			
			bl = new BulkLoader("main-site");
			bl.addEventListener(BulkProgressEvent.COMPLETE, init);
			bl.add("asset/wood01.jpg", {id:"0"});
			bl.start();
		}
		
		private function showInfo():void
		{
			var fps:FPS = new FPS(scene, viewport);
			addChild(fps);
			
			var tformat:TextFormat = new TextFormat("arial", 12, 0xffffff, null, null, null, null, null, "right");
			var tf2:TextField = new TextField();
			tf2.defaultTextFormat = tformat;
			tf2.autoSize = "right";
			tf2.text = "Alchemy3D\nAuthor : 牛牛猪 / LinFuQing";
			tf2.selectable = false;
			tf2.x = 440;
			tf2.y = 363;
			addChild(tf2);
		}
		
		protected function init(e:Event = null):void
		{
			t1 = new Texture(bl.getBitmapData("0"));
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(0.2, 0.2, 0.2, 1);
			m.diffuse = new ColorTransform(.7, .7, 0, 1);
			m.specular = new ColorTransform(1, 1, 0, 1);
			m.power = 48;
			
			var lightM:Material = new Material();
			lightM.ambient = new ColorTransform(1, 1, 0, 1);
			lightM.diffuse = new ColorTransform(1, 1, 0, 1);
			
			scene = new Scene3D();
			addScene(scene);
			
			camera = new Camera3D(0, 90, 100, 5000);
			addCamera(camera);
			camera.z = -400;
			
			viewport = new Viewport3D(640, 480, scene, camera);
			addViewport(viewport);
			
			lightObj = new Sphere(lightM, null, 10, 3, 2)
			scene.addEntity(lightObj);
			lightObj.renderMode = RenderMode.RENDER_WIREFRAME_TRIANGLE_32;
			
			p = new Plane(m, t1, 700, 500, 20, 10);
			scene.addEntity(p);
			p.lightEnable = true;
			p.renderMode = RenderMode.RENDER_TEXTRUED_TRIANGLE_GSINVZB_32;
			p.rotationX = 30;
			p.rotationY = 15;
			p.z = 400;
			
			light = new Light3D(lightObj);
			scene.addLight(light);
			light.type = LightType.POINT_LIGHT;
			light.mode = LightType.HIGH_MODE;
			light.bOnOff = LightType.LIGHT_ON;
			light.source.y = 300;
			light.source.x = 0;
			light.source.z = 480;
			light.ambient = new ColorTransform(0.4, 0.4, 0, 1);
			light.diffuse = new ColorTransform(.7, .7, 0, 1);
			light.specular = new ColorTransform(1, 1, 0, 1);
			light.attenuation1 = .002;
			light.attenuation2 = .00001;
			
			showInfo();
			
			startRendering();
			
//			moveLight1(1);
		}
		
		protected function moveLight1(dir:int = 1):void
		{
			var targetX:int = 300 * dir + 100;
			var targetY:int = 200 - 200 * dir;
			var targetZ:int = 100 * dir + 480;
			TweenLite.to(lightObj, 3, {delay:1.5, x:targetX, y:targetY, z:targetZ, onComplete:moveLight1, onCompleteParams:[dir * -1]});
		}
		
		private var t:Number = 0;
		override protected function onRenderTick(e:Event = null):void
		{
			camera.target = p.worldPosition;
			var mx:Number = viewport.mouseX / 300;
			var my:Number = - viewport.mouseY / 600;
			
			camera.hover(mx, my, 10);
			
			var a:Number;
			var i:int = 0;
			var vertices:Vector.<Vertex3D> = p.vertices;
			for ( ; i < p.nVertices; i ++ )
			{
				p.setVerticesZ(i, Math.sin( ( t + vertices[i].x * 0.02 ) ) * 30);
			}
			
			t += .2;
			
			super.onRenderTick(e);
		}
	}
}