package cn.alchemy3d.geom
{
	import __AS3__.vec.Vector;
	
	import cn.alchemy3d.base.Pointer;
	
	public class Polygon extends Pointer
	{
		internal var vertices:Vector.<Vertex>;
		internal var uvs     :Vector.<UV>;
		
		public function Polygon( vertices:Vector.<Vertex>, uvs:Vector.<UV> )
		{
			super();
			
			if( !vertices )
			{
				throw new Error( "No vertices!" );
			}
			
			if( uvs )
			{
				if( uvs.length != vertices.length )
				{
					throw new Error( "Uvs must match vertices!" );
				}
			}
			
			this.vertices = vertices;
			this.uvs      = uvs;
		}

	}
}