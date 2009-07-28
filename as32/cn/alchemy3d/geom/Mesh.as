package cn.alchemy3d.geom
{
	import __AS3__.vec.Vector;
	
	import cn.alchemy3d.base.Instance;
	
	public class Mesh extends Instance
	{
		private var _vertices:uint;
		private var _faces   :uint;
		
		private function set vertices( vs:Vector.<Vertex> ):void
		{
			var v     :Vertex; 
			var vertex:Array;
				
			var vl    :uint = vs.length;
			var vp    :uint = instance.library.applyForTmpBuffer( vl * 3 * instance.library.doubleTypeSize() );
			 
			if( _vertices )
			{
				instance.library.destoryVertices( _vertices );
			}

			instance.buffer.position = vp;
			
			for each ( v in vs )
			{
				instance.buffer.writeDouble( v.x );
				instance.buffer.writeDouble( v.y );
				instance.buffer.writeDouble( v.z );
			}
			
			var pointer:Array        = instance.library.initializeVertices( vp, vl );
			
			_vertices                = pointer[0];
			
			instance.buffer.position = pointer[1];

			for each( v in vs )
			{
				v.pointer       = instance.buffer.readUnsignedInt();
				
				vertex          = instance.library.initializeVertex( v.pointer );
				
				v.xPointer      = vertex[0];
				v.yPointer      = vertex[1];
				v.zPointer      = vertex[2];
				
				v.worldXPointer = vertex[3];
				v.worldYPointer = vertex[4];
				v.worldZPointer = vertex[5];
				
				instance.buffer.position ++;
			}
		}
		
		private function set faces( fs:Vector.<Polygon> ):void
		{
			buffer.position = tmpBuffPointer + vertices.length * 4 * sizeOfType;
			
			var f:Polygon;
			var i:uint;
			var fl:uint;
			
			for each ( f in faces )
			{
				fl = f.vertices.length;
				
				for( i = 0; i < fl; i ++ )
				{
					if( !f.vertices[i].pointer )
					{
						throw new Error( "Do not match vertices!" );
					}
					
					instance.buffer.writeUnsignedInt( f.vertices[i].pointer );
					
					instance.buffer.writeDouble( f.uvs[i].u );
					instance.buffer.writeDouble( f.uvs[i].v );
				}
			}
		}
		
		public function Mesh( vertices:Vector.<Vertex>, faces:Vector.<Polygon> )
		{
			super();
			
			if( !vertices )
			{
				throw new Error( "No vertices!" );
			}
			
			this.vertices = vertices;
			this.faces    =    faces;
			
			pointer       = instance.library.initializeMesh( _vertices, _faces );
		}
	}
}