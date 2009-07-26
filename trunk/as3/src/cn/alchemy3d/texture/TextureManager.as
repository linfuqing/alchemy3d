package cn.alchemy3d.texture
{
	/**
	* 材质管理器
	* <p/>
	* 每个材质管理器拥有自身的材质列表
	*/
	public class TextureManager
	{
		
		/**
		* [internal-use] 材质列表
		*/
		protected var _materials:Vector.<Texture>;
	
		private var _materialsTotal:int;

		/**
		* 材质数量
		*/
		public function get numMaterials():int
		{
			return _materialsTotal;
		}
		
		/**
		* 材质列表
		*/
		public function get materials():Vector.<Texture>
		{
			return _materials;
		}
		
		/**
		* 材质管理器构造函数
		*/
		public function TextureManager(materials:Vector.<Texture> = null)
		{
			if(materials)
			{
				this._materials = materials;
				this._materialsTotal  = _materials.length;
			}
			else
			{
				this._materials = new Vector.<Texture>;
				this._materialsTotal  = 0;
			}
		}
	
		/**
		* 添加材质到管理器
		*
		* @param Texture 被添加的材质实例
		* @param name 材质名称
		* @return 返回被添加的材质实例
		*/
		public function addMaterial(texture:Texture, name:String=null):Texture
		{
			texture.name = name || texture.name;
			
			this._materials.push(texture);
			//this._materials[Texture] = name;
			//this.materialsByName[name] = Texture;
			this._materialsTotal ++;
	
			return texture;
		}
	
		/**
		* 从指定索引位置删除材质实例
		*
		* @param Texture 索引值
		* @return 返回被删除的材质实例
		*/
		public function removeMaterialAt(index:int):Texture
		{
			if (index >= 0 && index < _materials.length)
				return this._materials.splice(index, 1)[0];
			else
				throw new Error("索引超出了范围，没有找到子对象");
		}
		
		public function getMaterialAt(index:int):Texture
		{
			if (index >= 0 && index < _materials.length)
				return this._materials[index];
			else
				throw new Error("索引超出了范围，没有找到子对象");
		}
	
		/**
		* 返回指定名称的材质实例
		* 
		* @param name 材质名称
		* @return 返回材质实例
		*/
		public function getMaterialByName(name:String):Texture
		{
			for(var i:int = 0; i < _materials.length; i ++)
			{
				if (name == _materials[i].name)
					return _materials[i];
			}
			return null;
		}
	
		/**
		* 删除指定名称的材质实例
		* 
		* @param name 材质名称
		* @return 返回被删除的材质实例
		*/
		public function removeMaterialByName(name:String):Texture
		{
			for(var i:int = 0; i < _materials.length; i ++)
			{
				if (name == _materials[i].name)
					return removeMaterialAt(i);
			}
			return null;
		}
	
		/**
		* 删除指定材质实例
		* 
		* @param name 材质实例
		*/
		public function removeMaterial(texture:Texture):Texture
		{
			var i:int = _materials.indexOf(texture);
			if(i > -1)
				return _materials.splice(i, 1)[0];
			else
				return null;
		}
	
		/**
		* 以字符串形式返回管理器内所有材质名称
		*
		* @return 返回字符串
		*/
		public function toString():String
		{
			var list:String = "";
	
			var m:Texture;
			for each(m in this._materials)
				list += this._materials[ m ] + "\n";
	
			return list;
		}
	}
}