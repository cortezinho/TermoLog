import * as FileSystem from 'expo-file-system';
import * as Sharing from 'expo-sharing';

export const exportarRelatorio = async (dados) => {
  let csv = "Data/Hora,Temperatura(C)\n";
  dados.forEach(log => {
    csv += `${log.hora},${log.temp}\n`;
  });
  const path = `${FileSystem.documentDirectory}relatorio.csv`;
  await FileSystem.writeAsStringAsync(path, csv);
  await Sharing.shareAsync(path);
};