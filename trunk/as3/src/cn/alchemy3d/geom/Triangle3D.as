package cn.alchemy3d.geom
{

	import cn.alchemy3d.objects.Entity;
	
	import flash.geom.Point;
	import flash.geom.Vector3D;
	
	public class Triangle3D extends AbstractRenderable
	{
		public function Triangle3D(aIndex:int, bIndex:int, cIndex:int, uva:Point, uvb:Point, uvc:Point, instance:Entity = null, name:String = "")
		{
			super(instance, name);
			
			p0Index = aIndex;
			p1Index = bIndex;
			p2Index = cIndex;
			
			uv0 = uva;
			uv1 = uvb;
			uv2 = uvc;
			
			uvtData = new Vector.<Number>(6, true);
			
			uvtData[0] = uva.x;
			uvtData[1] = uva.y;
			uvtData[2] = uvb.x;
			uvtData[3] = uvb.y;
			uvtData[4] = uvc.x;
			uvtData[5] = uvc.y;
		}
		
		public var normal:Vector3D;
		
		public var p0Index:int;
		public var p1Index:int;
		public var p2Index:int;
		
		public var uv0:Point;
		public var uv1:Point;
		public var uv2:Point;
		
		public var uvtData:Vector.<Number>;
		
		public function destroy():void
		{
			uvtData.length = 0;
			uvtData = null;
		}
		
		public function toString():String
		{
			if (this.name)
				return this.name;
			else
				return "[Triangle3D]";
		}

	}
}