import Foundation

extension Int8 {
  var s: Int { return Int(self) }
  var gf: CGFloat { return CGFloat(self) }
}

extension Int16 {
  var s: Int { return Int(self) }
  var gf: CGFloat { return CGFloat(self) }
}

extension Int32 {
  var s: Int { return Int(self) }
  var gf: CGFloat { return CGFloat(self) }
}

extension UInt8  {
  var s: UInt { return UInt(self) }
  var gf: CGFloat { return CGFloat(self) }
}

extension UInt16 {
  var s: UInt { return UInt(self) }
  var gf: CGFloat { return CGFloat(self) }
}

extension UInt32 {
  var s: UInt { return UInt(self) }
  var gf: CGFloat { return CGFloat(self) }
}

extension CFloat {
  var s: Float { return Float(self) }
  var gf: CGFloat { return CGFloat(self) }
}

extension CDouble {
  var s: Double { return Double(self) }
  var gf: CGFloat { return CGFloat(self) }
}

extension Int {
  var c: CInt { return CInt(self) }
}

extension Float {
  var c: CFloat { return CFloat(self) }
}

extension Double {
  var c: CDouble { return CDouble(self) }
}

extension CGFloat {
  var cf: CFloat { return CFloat(self) }
  var ci: CInt { return CInt(self) }
  var f: Float { return Float(self) }
  var i: Int { return Int(self) }
}
