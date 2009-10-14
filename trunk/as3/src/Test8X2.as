package
{
	import br.com.stimuli.loading.BulkLoader;
	import br.com.stimuli.loading.BulkProgressEvent;
	
	import cn.alchemy3d.container.Entity;
	import cn.alchemy3d.external.MD2;
	import cn.alchemy3d.external.Primitives;
	import cn.alchemy3d.fog.Fog;
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.lights.LightType;
	import cn.alchemy3d.render.Material;
	import cn.alchemy3d.render.RenderMode;
	import cn.alchemy3d.render.Texture;
	import cn.alchemy3d.terrain.MeshTerrain;
	import cn.alchemy3d.tools.Basic;
	import cn.alchemy3d.tools.FPS;
	
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.geom.ColorTransform;
	import flash.net.URLLoader;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.ui.Keyboard;
	
	import gs.TweenLite;

	[SWF(width="600",height="400",backgroundColor="#70782c",frameRate="60")]
	public class Test8X2 extends Basic
	{
		protected var terrain:MeshTerrain;
		
		protected var lightObj2:Primitives;
		
		protected var light2:Light3D;
		
		protected var center:Entity;
		
		public function Test8X2()
		{
			super(600, 400, 90, 20, 8000);
			
			init();
		}
		
		private function showInfo():void
		{
			var fps:FPS = new FPS(scene);
			addChild(fps);
		}
		
		protected function init(e:Event = null):void
		{
			camera.x = -810;
			
//			center = new Entity();
//			center.z = 0;
//			viewport.scene.addChild(center);
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(1, 1, 1, 1);
			m.diffuse = new ColorTransform(.4, .4, .4, 1);
			m.specular = new ColorTransform(0, 0, 0, 1);
			
			var m2:Material = new Material();
			m2.ambient = new ColorTransform(0, .2, 0, 1);
			m2.diffuse = new ColorTransform(0, 1, 0, 1);
			m2.specular = new ColorTransform(0, 0, 0, 1);
			m2.power = 0;
			
			lightObj2 = new Primitives(m, null, RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32);
			lightObj2.toPlane(150, 150, 1, 1);
			lightObj2.y = 2000;
			lightObj2.x = 0;
			lightObj2.z = 2000;
			lightObj2.mesh.terrainTrace = true;
			viewport.scene.addChild(lightObj2);
			
			terrain = new MeshTerrain(null, m2, null, RenderMode.RENDER_GOURAUD_TRIANGLE_INVZB_32);
			terrain.buildOn(10000, 10000, 3000);
			terrain.mesh.lightEnable = true;
			terrain.z = 0;
			terrain.y = -30;
			
			viewport.scene.addChild(terrain);

			
			light2 = new Light3D(lightObj2);
			viewport.scene.addLight(light2);
			light2.type = LightType.POINT_LIGHT;
			light2.mode = LightType.MID_MODE;
			light2.bOnOff = LightType.LIGHT_ON;
			light2.ambient = new ColorTransform(0, 0, 0, 1);
			light2.diffuse = new ColorTransform(0, 1, 0, 1);
			light2.specular = new ColorTransform(0, 0, 0, 1);
			light2.attenuation1 = 0.0002;
			light2.attenuation2 = 0;

			startRendering();
			
			showInfo();
			
			moveLight2(1);
			
			stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
		}
		
		protected function onKeyDown(e:KeyboardEvent):void
		{
			if ( e.keyCode == Keyboard.UP )
				camera.z += 10;
				
			if ( e.keyCode == Keyboard.DOWN )
				camera.z -= 10;
		}
		
		protected function moveLight2(dir:int = 1):void
		{
			var target:int = 2000 * dir;
			TweenLite.to(lightObj2, 8, { x:target, onComplete:moveLight2, onCompleteParams:[dir * -1]});
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
			super.onRenderTick(e);
			
//			camera.target = center.worldPosition;

			var mx:Number = viewport.mouseX / 30;
			var my:Number = - viewport.mouseY / 30;
			
			camera.hover(mx, my, 10);
		}
	}
}