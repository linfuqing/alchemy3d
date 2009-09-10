package cn.alchemy3d.geom
{

	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.texture.Texture;
	
	import flash.geom.Point;
	
	public class Triangle3D extends AbstractRenderable
	{
		internal var ID:uint;
		
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
			super(instance, name);
			
			this.v0 = v0;
			this.v1 = v1;
			this.v2 = v2;
			
			uv0 = uva;
			uv1 = uvb;
			uv2 = uvc;
			
			
			this.material = material;
			this.texture  = texture;
			
			this.renderMode = renderMode;
			
			/*uvtData = new Vector.<Number>(6, true);
			
			uvtData[0] = uva.x;
			uvtData[1] = uva.y;
			uvtData[2] = uvb.x;
			uvtData[3] = uvb.y;
			uvtData[4] = uvc.x;
			uvtData[5] = uvc.y;*/
		}
		
		public var v0:Vertex3D;
		public var v1:Vertex3D;
		public var v2:Vertex3D;
		
		public var uv0:Point;
		public var uv1:Point;
		public var uv2:Point;
		
		//public var uvtData:Vector.<Number>;
		
		public var material:Material;
		public var texture:Texture;
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
			if (this.name)
				return this.name;
			else
				return "[Triangle3D]";
		}

	}
}