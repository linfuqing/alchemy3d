package cn.alchemy3d.geom
{

	import cn.alchemy3d.render.Material;
	import cn.alchemy3d.render.Texture;
	
	import flash.geom.Point;
	
	public class Triangle3D
	{
		//internal var ID:uint;
		
		public function get material():Material
		{
			return _material;
		}
		
		public function get texture():Texture
		{
			return _texture;
		}
		
		public function Triangle3D(
		v0:Vertex3D, 
		v1:Vertex3D, 
		v2:Vertex3D, 
		uva:Point, 
		uvb:Point, 
		uvc:Point, 
		material:Material, 
		texture:Texture = null, 
		renderMode:int = 0x000000 )
		{
			this.v0 = v0;
			this.v1 = v1;
			this.v2 = v2;
			
			uv0 = uva;
			uv1 = uvb;
			uv2 = uvc;
			
			
			_material = material;
			_texture  = texture;
			
			this.renderMode = renderMode;
			
			/*uvtData = new Vector.<Number>(6, true);
			
			uvtData[0] = uva.x;
			uvtData[1] = uva.y;
			uvtData[2] = uvb.x;
			uvtData[3] = uvb.y;
			uvtData[4] = uvc.x;
			uvtData[5] = uvc.y;*/
		}
		
		internal var v0:Vertex3D;
		internal var v1:Vertex3D;
		internal var v2:Vertex3D;
		
		internal var uv0:Point;
		internal var uv1:Point;
		internal var uv2:Point;
		
		//public var uvtData:Vector.<Number>;
		
		internal var _material:Material;
		internal var _texture:Texture;
		
		public var renderMode:int;
		
//		public function clone():Triangle3D
//		{
//			return new Triangle3D(p0Index:int
//		}
		
		/*public function destroy():void
		{
			uvtData.length = 0;
			uvtData = null;
		}*/
		
		public function toString():String
		{
			return "[Triangle3D]";
		}

	}
}