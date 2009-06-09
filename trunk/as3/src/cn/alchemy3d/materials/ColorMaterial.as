package cn.alchemy3d.materials
{
	/**
	 * 颜色材质
	 */
	public class ColorMaterial extends AbstractMaterial
	{
		/**
		 * 构造函数
		 * 
		 * @param color 线条颜色
		 * @param alpha 线条透明度
		 */
		public function ColorMaterial(color:uint = 0xffffff, alpha:int = 255)
		{
			super();
			
			this.type = 1;
			this.color = color;
			this.alpha = alpha;
			this.ready = true;
		}
		
		/**
		 * [internal-use]返回当前材质的一个副本
		 */
		override public function clone():AbstractMaterial
		{
			return new ColorMaterial(this.color, this.alpha);
		}
	}
}