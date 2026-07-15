import { useColorScheme as useColorSchemeRN } from "react-native";

const useColorScheme = (): "light" | "dark" =>
  useColorSchemeRN() === "dark" ? "dark" : "light";

export default useColorScheme;
